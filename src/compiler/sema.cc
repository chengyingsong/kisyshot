#include <compiler/sema.h>
#include <cassert>

namespace kisyshot::compiler {

    Sema::Sema(
        const std::shared_ptr<Context>& context,
        const std::shared_ptr<diagnostic::DiagnosticStream>& diagStream) {
        _context = context;
        _diagnosticStream = diagStream;
    }

    void Sema::check() { traverse(); }

    void Sema::traverse() {
        // get global layer symbols
        for (auto& node : _context->syntaxTree->children) {
            if (node->getType() ==
                ast::syntax::SyntaxType::VariableDeclaration) {
                auto decl =
                    std::dynamic_pointer_cast<ast::syntax::VarDeclaration>(
                        node);
                for (auto& def : decl->varDefs) {
                    _globals[def->varName->identifier] = def;
                    _context->symbols[def->varName->identifier] = def;
                    _context->globals.push_back(def);
                }
            }
            if (node->getType() == ast::syntax::SyntaxType::Function) {
                auto func =
                    std::dynamic_pointer_cast<ast::syntax::Function>(node);
                if (_context->functions.count(func->name->identifier) == 1) {
                    if (func->body == nullptr)
                        continue;
                    auto prev = _context->functions[func->name->identifier];
                    if (prev->body == nullptr) {
                        std::swap(prev->body, func->body);
                        continue;
                    }
                    // TODO: push error
                }
                _context->functions[func->name->identifier] = func;
            }
        }
        // init all mangled names
        for (auto& [_, def] : _context->symbols) {
            if (def->initialValue != nullptr)
                traverseExpression(def->initialValue);
        }
        // check & compute global consts
        int value;
        bool ok;
        for (auto& def : _context->globals) {
            if (def->dimensionDef.empty()) {
                std::tie(value, ok) =
                        checkCompileTimeConstExpr(def->initialValue);
                def->values.emplace_back(ok ? value : 0);
                def->accumulation.push_back(1);
            } else {
                prepareArrayDef(def);

                if (def->initialValue != nullptr) {
                    flattenArray(def,
                                 std::dynamic_pointer_cast<
                                         ast::syntax::ArrayInitializeExpression>(
                                         def->initialValue),
                                 def->accumulation.front());
                    for (auto &expr : def->srcArray) {

                        std::tie(value, ok) = checkCompileTimeConstExpr(expr);
                        def->values.emplace_back(ok ? value : 0);
                    }
                }
            }
        }
        // init functions
        for (auto& [_, func] : _context->functions) {
            // push function layer name
            if (func->body != nullptr) {
                _layerNames.push_back(func->name->identifier);
                std::vector<std::string> recover;
                for (auto &param : func->params) {
                    prepareArrayDef(param);
                    param->offset = func->stackSize;
                    func->stackSize += 4;
                    auto id = param->varName->identifier;
                    param->varName->mangledId = id + "%" +func->name->identifier;
                    _context->symbols[param->varName->mangledId] = param;
                    if (_globals.count(param->varName->identifier) == 1) {
                        recover.push_back(id);
                    }
                    _globals[id] = param;
                }
                _layerNames.pop_back();
                _blockName = func->name->identifier;
                _currFunc = func;
                traverseStatement(func->body);
                // release param covered global ids;
                for(auto& id: recover){
                    _globals[id] = _context->symbols[id];
                }
            }
        }

    }

    void Sema::traverseExpression(
        const std::shared_ptr<ast::syntax::Expression>& expr) {
        switch (expr->getType()) {
            case ast::syntax::SyntaxType::BinaryExpression: {
                auto&& e =
                    std::dynamic_pointer_cast<ast::syntax::BinaryExpression>(
                        expr);
                traverseExpression(e->left);
                traverseExpression(e->right);
                break;
            }
            case ast::syntax::SyntaxType::UnaryExpression: {
                auto&& e =
                    std::dynamic_pointer_cast<ast::syntax::UnaryExpression>(
                        expr);
                traverseExpression(e->right);
                break;
            }
            case ast::syntax::SyntaxType::ParenthesesExpression: {
                auto&& e = std::dynamic_pointer_cast<
                    ast::syntax::ParenthesesExpression>(expr);
                traverseExpression(e->innerExpression);
                break;
            }
            case ast::syntax::SyntaxType::IndexExpression: {
                auto&& e =
                    std::dynamic_pointer_cast<ast::syntax::IndexExpression>(
                        expr);

                traverseExpression(e->indexedExpr);
                traverseExpression(e->indexerExpr);

                e->accumulation = _context->symbols[e->arrayName->mangledId]->accumulation[e->layer];
                break;
            }
            case ast::syntax::SyntaxType::ArrayInitializeExpression: {
                for (auto&& e :
                     std::dynamic_pointer_cast<
                         ast::syntax::ArrayInitializeExpression>(expr)
                         ->array) {
                    traverseExpression(e);
                }
                break;
            }
            case ast::syntax::SyntaxType::CallExpression: {
                auto&& e =
                    std::dynamic_pointer_cast<ast::syntax::CallExpression>(
                        expr);
                if (_context->functions.count(e->name->identifier) == 1) {
                    e->name->mangledId = e->name->identifier;
                } else {
                    // TODO: push error
                }
                for (auto&& arg : e->arguments) {
                    traverseExpression(arg);
                }
                break;
            }
            case ast::syntax::SyntaxType::IdentifierExpression: {
                auto&& e = std::dynamic_pointer_cast<
                    ast::syntax::IdentifierExpression>(expr);
                auto id = e->name->identifier;
                auto&& s = _variables[id];
                if (s.empty()) {
                    assert(_globals.count(id) == 1);
                    e->name->mangledId = _globals[e->name->identifier]->varName->mangledId;
                    break;
                }
                e->name->mangledId = s.top()->varName->mangledId;
                break;
            }
            case ast::syntax::SyntaxType::StringLiteralExpression: {
                auto&& s = std::dynamic_pointer_cast<
                        ast::syntax::StringLiteralExpression>(expr);
                auto str = (std::string) s->rawCode;
                if (_context->strings.count(str) == 0){
                    s->label = ".LC" + std::to_string(_context->strings.size());
                    _context->strings[str] = s->label;
                } else{
                    s->label = _context->strings[str];
                }
            }
            default:
                break;
        }
    }

    std::pair<int, bool> Sema::checkCompileTimeConstExpr(
        const std::shared_ptr<ast::syntax::Expression>& expr) {
        if (expr == nullptr)
            return std::make_pair(0, false);
        switch (expr->getType()) {
            case ast::syntax::SyntaxType::NumericLiteralExpression: {
                return std::make_pair(std::dynamic_pointer_cast<ast::syntax::NumericLiteralExpression>(expr)->number,
                                      true);
            }
            case ast::syntax::SyntaxType::UnaryExpression: {
                auto unary =
                    std::dynamic_pointer_cast<ast::syntax::UnaryExpression>(
                        expr);
                int right;
                bool ok;
                std::tie(right, ok) = checkCompileTimeConstExpr(unary->right);
                if (!ok) {
                    return std::make_pair(right, ok);
                }
                switch (unary->operatorType) {
                    case ast::TokenType::op_plus:
                        return std::make_pair(right, true);
                    case ast::TokenType::op_minus:
                        return std::make_pair(-right, true);
                    case ast::TokenType::op_exclaim:
                        return std::make_pair(!right, true);
                    default:
                        // TODO: push unknown unary expr
                        return std::make_pair(0, false);
                }
            }
            case ast::syntax::SyntaxType::BinaryExpression: {
                auto binary =
                    std::dynamic_pointer_cast<ast::syntax::BinaryExpression>(
                        expr);
                int left, right;
                bool ok;
                std::tie(left, ok) = checkCompileTimeConstExpr(binary->left);
                if (!ok) {
                    return std::make_pair(left, ok);
                }
                std::tie(right, ok) = checkCompileTimeConstExpr(binary->right);
                if (!ok) {
                    return std::make_pair(right, ok);
                }

                switch (binary->operatorType) {
                    case ast::TokenType::op_plus:
                        return std::make_pair(left + right, true);
                    case ast::TokenType::op_minus:
                        return std::make_pair(left - right, true);
                    case ast::TokenType::op_multi:
                        return std::make_pair(left * right, true);
                    case ast::TokenType::op_slash:
                        return std::make_pair(left / right, true);
                    case ast::TokenType::op_modulus:
                        return std::make_pair(left % right, true);
                    case ast::TokenType::op_less:
                        return std::make_pair(left < right, true);
                    case ast::TokenType::op_greater:
                        return std::make_pair(left > right, true);
                    case ast::TokenType::op_equaleq:
                        return std::make_pair(left == right, true);
                    case ast::TokenType::op_exclaimeq:
                        return std::make_pair(left != right, true);
                    case ast::TokenType::op_greatereq:
                        return std::make_pair(left >= right, true);
                    case ast::TokenType::op_lesseq:
                        return std::make_pair(left <= right, true);
                    default:
                        // TODO: push unknown unary expr
                        return std::make_pair(0, false);
                }
            }
            case ast::syntax::SyntaxType::IdentifierExpression: {
                auto name =
                        std::dynamic_pointer_cast<ast::syntax::IdentifierExpression>(
                                expr);
                auto id = name->name->identifier;
                auto&& s = _variables[id];
                std::shared_ptr<ast::syntax::VarDefinition> def;
                if (s.empty()) {
                    assert(_globals.count(id) == 1);
                    def = _globals[id];
                } else{
                    def = s.top();
                }
                if (def->isConst) {
                    return checkCompileTimeConstExpr(def->initialValue);
                }
                // TODO: push 'is not a compile-time constant''
                return std::make_pair(0, false);
            }
            default:
                // TODO: push 'is not a compile-time constant'
                return std::make_pair(0, false);
        }
    }

    void Sema::traverseStatement(
        const std::shared_ptr<ast::syntax::Statement>& stmt) {
        switch (stmt->getType()) {
            case ast::syntax::SyntaxType::VariableDeclaration: {
                for (auto& def :
                     std::dynamic_pointer_cast<ast::syntax::VarDeclaration>(
                         stmt)
                         ->varDefs) {
                    prepareArrayDef(def);

                    newVariable(def);
                    if (def->initialValue != nullptr) {
                        traverseExpression(def->initialValue);
                    }

                    def->offset = _currFunc->stackSize;
                    size_t s = 1;
                    for (auto&& i : def->dimension) {
                        // TODO : push error
                        assert(i > 0);
                        s *= i;
                    }

                    if (!def->dimensionDef.empty()) {
                        flattenArray(
                            def,
                            std::dynamic_pointer_cast<
                                ast::syntax::ArrayInitializeExpression>(
                                def->initialValue),
                            s);
                        _currFunc->stackSize += def->srcArray.size() * 4;
                    } else {
                        _currFunc->stackSize += 4;
                    }
                }
                break;
            }
            case ast::syntax::SyntaxType::BlockStatement: {
                // add a block-exit cleaner
                _blockVars.emplace();
                if (_blockName.empty()) {
                    _layerNames.emplace_back("b." + std::to_string(_blockId++) +
                                             "@" + _layerNames.back());
                } else {
                    _layerNames.push_back(_blockName);
                    _blockName.clear();
                }
                for (auto& s :
                     std::dynamic_pointer_cast<ast::syntax::BlockStatement>(
                         stmt)
                         ->children) {
                    traverseStatement(s);
                }
                _layerNames.pop_back();
                for (auto& id : _blockVars.top()) {
                    _variables[id].pop();
                }
                // pop block
                _blockVars.pop();
                break;
            }
            case ast::syntax::SyntaxType::IfStatement: {
                auto&& s =
                    std::dynamic_pointer_cast<ast::syntax::IfStatement>(stmt);
                // traverse expressions
                _blockName = "i." + std::to_string(_blockId++) + "@" +
                             _layerNames.back();
                traverseExpression(s->condition);
                traverseStatement(s->ifClause);
                if (s->elseClause != nullptr) {
                    _blockName = "e." + std::to_string(_blockId++) + "@" +
                                 _layerNames.back();
                    traverseStatement(s->elseClause);
                }
                break;
            }
            case ast::syntax::SyntaxType::WhileStatement: {
                auto&& s =
                    std::dynamic_pointer_cast<ast::syntax::WhileStatement>(
                        stmt);
                // traverse expressions
                traverseExpression(s->condition);
                _blockName = "w." + std::to_string(_blockId++) + "@" +
                             _layerNames.back();
                traverseStatement(s->body);
                break;
            }
            case ast::syntax::SyntaxType::ExpressionStatement: {
                traverseExpression(
                    std::dynamic_pointer_cast<ast::syntax::ExpressionStatement>(
                        stmt)
                        ->expression);
                break;
            }
            case ast::syntax::SyntaxType::ReturnStatement: {
                auto&& s =
                    std::dynamic_pointer_cast<ast::syntax::ReturnStatement>(
                        stmt);
                if (s->value != nullptr)
                    traverseExpression(s->value);
                break;
            }
            default:
                break;
        }
    }

    void
    Sema::newVariable(const std::shared_ptr<ast::syntax::VarDefinition>& def) {
        if (_variables.count(def->varName->identifier) == 1 &&
            _variables[def->varName->identifier].size() == _layerNames.size()) {
            // TODO error
            return;
        }
        if (_layerNames.empty()) {
            def->varName->mangledId = def->varName->identifier;
        } else {
            def->varName->mangledId =
                def->varName->identifier + "@" + _layerNames.back();
        }
        _context->symbols[def->varName->mangledId] = def;
        _variables[def->varName->identifier].push(def);
        _blockVars.top().push_back(def->varName->identifier);
    }

    void Sema::flattenArray(
        const std::shared_ptr<ast::syntax::VarDefinition>& def,
        const std::shared_ptr<ast::syntax::ArrayInitializeExpression>& init,
        size_t target, size_t dim) {
        if (dim >= def->dimension.size()) {
            // TODO: push array dimension wrong expr
        }
        size_t current = def->srcArray.size();
        size_t singleCount = 0;
        if (init != nullptr) {
            for (auto& i : init->array) {
                if (i->getType() ==
                    ast::syntax::SyntaxType::ArrayInitializeExpression) {
                    size_t innerTarget = target / def->dimension[dim];
                    if (singleCount != 0) {
                        assert(singleCount == innerTarget);
                        singleCount = 0;
                    }

                    flattenArray(def,
                                 std::dynamic_pointer_cast<
                                     ast::syntax::ArrayInitializeExpression>(i),
                                 innerTarget, dim + 1);
                } else {
                    def->srcArray.push_back(i);
                    singleCount++;
                }
            }
        }
        while (def->srcArray.size() < current + target) {
            def->srcArray.push_back(std::make_shared<ast::syntax::NumericLiteralExpression>());
        }
    }

    void Sema::prepareArrayDef(const std::shared_ptr<ast::syntax::VarDefinition> &def) {
        int value;
        bool ok;
        for (auto &dimExpr : def->dimensionDef) {
            std::tie(value, ok) = checkCompileTimeConstExpr(dimExpr);
            if (!ok) {
                value = 0;
            }
            def->dimension.push_back(value);
        }
        auto dim = def->dimension;
        def->accumulation.push_back(1);
        while (!dim.empty()){
            def->accumulation.push_back(def->accumulation.back() * dim.back());
            dim.pop_back();
        }
        std::reverse(def->accumulation.begin(), def->accumulation.end());
        def->accumulation.back() = -1;
    }

} // namespace kisyshot::compiler