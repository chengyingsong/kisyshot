#include <compiler/sema.h>

namespace kisyshot::compiler{


    Sema::Sema(const std::shared_ptr<Context> &context,
               const std::shared_ptr<diagnostic::DiagnosticStream> &diagStream) {
        _context = context;
        _diagnosticStream = diagStream;
    }

    void Sema::check() {
        genSymbolTable();
        bindSymbols();
    }

    void Sema::genSymbolTable() {

    }

    void Sema::bindSymbols() {

    }

    void Sema::traverse() {
        for (auto& node: _context->syntaxTree->children) {
            if (node->getType() == ast::syntax::SyntaxType::VariableDeclaration){
                auto decl = std::dynamic_pointer_cast<ast::syntax::VarDeclaration>(node);
                for (auto &def:decl->varDefs) {
//                    newSymbol(def->varName, def);
                }
            }
            if (node->getType() == ast::syntax::SyntaxType::Function){
                auto func = std::dynamic_pointer_cast<ast::syntax::Function>(node);
                if (_context->functions.count(func->name->identifier) == 1){
                    if (func->body == nullptr)
                        continue;
                    auto prev = _context->functions[func->name->identifier];
                    if (prev->body == nullptr){
                        std::swap(prev->body, func->body);
                        continue;
                    }
                    // TODO: push error
                }
                _context->functions[func->name->identifier] = func;
            }
        }
        for (auto& node: _context->syntaxTree->children) {
            if (node->getType() == ast::syntax::SyntaxType::Function){

            }
        }
    }

//    void Sema::newSymbol(const std::shared_ptr<ast::syntax::Identifier> &id,
//                         const std::shared_ptr<ast::syntax::SyntaxNode> &node) {
//        if (_symbolDefinition[id->identifier].size() == _layer){
//            // TODO error
//            return;
//        }
//        id->mangledId = id->identifier;
//        if (!_layerNames.empty())
//            id->mangledId += "@" + _layerNames.back();
//        _context->symbols[id->mangledId] = node;
//        _symbolDefinition[id->identifier].push(node);
//    }

}