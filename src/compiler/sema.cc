#include <compiler/sema.h>

namespace kisyshot::compiler {

    Sema::Sema(
        const std::shared_ptr<Context>& context,
        const std::shared_ptr<diagnostic::DiagnosticStream>& diagStream) {
        _context = context;
        _diagnosticStream = diagStream;
    }

    void Sema::check() {
        traverse();
    }

    void Sema::traverse() {
        for (auto& node : _context->syntaxTree->children) {
            if (node->getType() ==
                ast::syntax::SyntaxType::VariableDeclaration) {
                // TODO: handle global variable offsets
                auto decl =
                    std::dynamic_pointer_cast<ast::syntax::VarDeclaration>(
                        node);
                for (auto& def : decl->varDefs) {
                    newVariable(def);
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
        for (auto& [_, def] : _context->symbols) {
            if(def->initialValue != nullptr)
                traverseExpression(def->initialValue);
        }
        for (auto& [_, func] : _context->functions) {
            // push function layer name
            _layerNames.push_back(func->name->identifier);
            auto& params = func->params;
            for (size_t i = 0; i < params.size(); i++) {
                if (i < 4) {
                    // reserve 4 param for register-based parameters.
                    params[i]->offset = 0;
                } else {
                    params[i]->offset = func->stackSize;
                    func->stackSize += 4;
                }
                newVariable(params[i]);
            }
            _layerNames.pop_back();
            _blockName = func->name->identifier;
            _currFunc = func;
            traverseStatement(func->body);
        }
    }

    void Sema::traverseExpression(const std::shared_ptr<ast::syntax::Expression>& expr) {
        switch (expr->getType()) {
            case ast::syntax::SyntaxType::BinaryExpression:{
                auto &&e = std::dynamic_pointer_cast<ast::syntax::BinaryExpression>(expr);
                traverseExpression(e->left);
                traverseExpression(e->right);
                break;
            }
            case ast::syntax::SyntaxType::UnaryExpression :{
                auto &&e = std::dynamic_pointer_cast<ast::syntax::UnaryExpression>(expr);
                traverseExpression(e->right);
                break;
            }
            case ast::syntax::SyntaxType::ParenthesesExpression :{
                auto &&e = std::dynamic_pointer_cast<ast::syntax::ParenthesesExpression>(expr);
                traverseExpression(e->innerExpression);
                break;
            }
            case ast::syntax::SyntaxType::IndexExpression :{
                auto &&e = std::dynamic_pointer_cast<ast::syntax::IndexExpression>(expr);

                _arrLayer++;

                traverseExpression(e->indexedExpr);
                traverseExpression(e->indexerExpr);
                if (_arrLayer != 1) {
                    auto def = _context->symbols[e->arrayName->mangledId];
                    e->layer = std::stoi((std::string) std::dynamic_pointer_cast<ast::syntax::NumericLiteralExpression>(
                            def->array[def->array.size() - _arrLayer + 1])
                            ->rawCode);
                }

                _arrLayer--;
                break;
            }
            case ast::syntax::SyntaxType::ArrayInitializeExpression :{
                for (auto &&e : std::dynamic_pointer_cast<ast::syntax::ArrayInitializeExpression>(expr)->array) {
                    traverseExpression(e);
                }
                break;
            }
            case ast::syntax::SyntaxType::CallExpression :{
                auto &&e = std::dynamic_pointer_cast<ast::syntax::CallExpression>(expr);
                if(_context->functions.count(e->name->identifier) == 1){
                    e->name->mangledId = e->name->identifier;
                } else {
                    // TODO: push error
                }
                for (auto &&arg : e->arguments) {
                    traverseExpression(arg);
                }
                break;
            }
            case ast::syntax::SyntaxType::IdentifierExpression :{
                auto &&e = std::dynamic_pointer_cast<ast::syntax::IdentifierExpression>(expr);
                auto &&s = _variables[e->name->identifier];
                if(s.empty()){
                    // TODO push er
                    break;
                }
                e->name->mangledId = s.top()->varName->mangledId;
            }
            default: break;
        }

    }
    void Sema::traverseStatement(const std::shared_ptr<ast::syntax::Statement>& stmt) { 
        switch (stmt->getType()) {
            case ast::syntax::SyntaxType::VariableDeclaration: {
                for (auto & def: std::dynamic_pointer_cast<ast::syntax::VarDeclaration>(stmt)->varDefs) {
                    newVariable(def);
                    if (def->initialValue != nullptr) {
                        traverseExpression(def->initialValue);

                        def->offset = _currFunc->stackSize;
                        if (!def->array.empty()) {
                            flattenInit(def, def->initialValue, 1);
                            _currFunc->stackSize += def->srcArray.size() * 4;
                        } else {
                            _currFunc->stackSize += 4;
                        }
                    }
                }
                break;
            }
            case ast::syntax::SyntaxType::BlockStatement: {
                if (_blockName.empty()){
                    _layerNames.emplace_back("b." + std::to_string(_blockId++) + "@" + _layerNames.back());
                } else {
                    _layerNames.push_back(_blockName);
                    _blockName.clear();
                }
                for (auto &s: std::dynamic_pointer_cast<ast::syntax::BlockStatement>(stmt)->children) {
                    traverseStatement(s);
                }
                _layerNames.pop_back();
                for (auto &&[_, s] : _variables) {
                    if(s.size() > _layerNames.size())
                        s.pop();
                }
                
                //
                break;
            }
            case ast::syntax::SyntaxType::IfStatement: {
                auto&& s = std::dynamic_pointer_cast<ast::syntax::IfStatement>(stmt);
                // traverse expressions 
                _blockName = "i." + std::to_string(_blockId++) + "@" + _layerNames.back();
                traverseExpression(s->condition);
                traverseStatement(s->ifClause);
                if (s->elseClause != nullptr){
                    _blockName = "e." + std::to_string(_blockId++) + "@" + _layerNames.back();
                    traverseStatement(s->elseClause);
                }
                break;
            }
            case ast::syntax::SyntaxType::WhileStatement: {
                auto&& s = std::dynamic_pointer_cast<ast::syntax::WhileStatement>(stmt);
                // traverse expressions 
                traverseExpression(s->condition);
                _blockName = "w." + std::to_string(_blockId++) + "@" + _layerNames.back();
                traverseStatement(s->body);
                break;
            }
            case ast::syntax::SyntaxType::ExpressionStatement: {
                traverseExpression(std::dynamic_pointer_cast<ast::syntax::ExpressionStatement>(stmt)->expression);
                break;
            }
            case ast::syntax::SyntaxType::ReturnStatement: {
                auto&& s = std::dynamic_pointer_cast<ast::syntax::ReturnStatement>(stmt);
                if(s->value != nullptr)
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
    }

    void Sema::flattenInit(const std::shared_ptr<ast::syntax::VarDefinition>& dst,const std::shared_ptr<ast::syntax::Expression> &srcExp, size_t dim) {
        size_t currNum = 0;
        if (dim > dst->array.size()) {
            //diagnostic output
            return;
        }
        for (std::shared_ptr<ast::syntax::Expression> &Exp : std::dynamic_pointer_cast<ast::syntax::ArrayInitializeExpression>(
                srcExp)->array) {
            if (Exp->getType() == ast::syntax::SyntaxType::ArrayInitializeExpression) {
                flattenInit(dst, Exp, dim + 1);
                currNum++;
            } else if (Exp->getType() == ast::syntax::SyntaxType::NumericLiteralExpression) {
                if (++currNum < dst->array[dim - 1]->start()) {
                    dst->srcArray.push_back(Exp);
                } else {
                    // diagnostic output
                    return;
                }
            }
        }
        if (dst->array.size() == dim) {
            for (; ++currNum < dst->array[dim - 1]->start();) {
                auto e = std::make_shared<ast::syntax::NumericLiteralExpression>();
                e->rawCode = "0";
                dst->srcArray.push_back(e);
            }
        } else {
            for (; ++currNum < dst->array[dim - 1]->start();) {
                flattenInit(dst, std::make_shared<ast::syntax::ArrayInitializeExpression>(), dim + 1);
            }
        }
    }
} // namespace kisyshot::compiler