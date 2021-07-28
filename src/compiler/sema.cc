#include <compiler/sema.h>

namespace kisyshot::compiler {

    Sema::Sema(
        const std::shared_ptr<Context>& context,
        const std::shared_ptr<diagnostic::DiagnosticStream>& diagStream) {
        _context = context;
        _diagnosticStream = diagStream;
    }

    void Sema::check() {
        genSymbolTable();
        bindSymbols();
    }

    void Sema::genSymbolTable() {}

    void Sema::bindSymbols() {}

    void Sema::traverse() {
        for (auto& node : _context->syntaxTree->children) {
            if (node->getType() ==
                ast::syntax::SyntaxType::VariableDeclaration) {
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
        for (auto& [_, func] : _context->functions) {
            // push function layer name
            _layerNames.push_back(func->name->identifier);

            auto& params = func->params->params;
            for (size_t i = 0; i < params.size(); i++) {
                if (i < 4) {
                    // reserve 4 param for register-based parameters.
                    params[i]->offset = 0;
                } else {
                    params[i]->offset = func->stackSize;
                    func->stackSize += 4;
                }
                // TODO push param decl
            }
            
            traverseStatement(func->body);
            _layerNames.pop_back();
        }
    }

    void Sema::traverseStatement(const std::shared_ptr<ast::syntax::Statement>& stmt){
        switch (stmt->getType()) {

            case ast::syntax::SyntaxType::BlockStatement:
                break;
            
            default:
                break;
        }
    }

    void
    Sema::newVariable(const std::shared_ptr<ast::syntax::VarDefinition>& def) {
        if (_variables.count(def->varName->identifier) == 1 &&
            _variables[def->varName->identifier].size() == _layer) {
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

} // namespace kisyshot::compiler