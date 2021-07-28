#pragma once

#include <context.h>
#include <diagnostic/diagnostic_stream.h>
#include <stack>

namespace kisyshot::compiler{
    class Sema{
    public:
        Sema(const std::shared_ptr<Context> &context,
             const std::shared_ptr<diagnostic::DiagnosticStream> &diagStream);
        void check();
    private:
        void genSymbolTable();
        void bindSymbols();
        void traverse();
        // copy of the context
        std::shared_ptr<Context> _context;
        // copy of the diagnostic info collector
        std::shared_ptr<diagnostic::DiagnosticStream> _diagnosticStream;

        std::unordered_map<std::string, std::stack<std::shared_ptr<ast::syntax::SyntaxNode>>> _symbolDefinition;
        std::vector<std::string> _layerNames;
        std::size_t _blockId;
        std::size_t _layer;
    };
}