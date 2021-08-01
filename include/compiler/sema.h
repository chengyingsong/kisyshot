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
        void traverse();
        void traverseExpression(const std::shared_ptr<ast::syntax::Expression>& expr);
        void traverseStatement(const std::shared_ptr<ast::syntax::Statement>& stmt);
        void newVariable(const std::shared_ptr<ast::syntax::VarDefinition>& def);
        void flattenInit(const std::shared_ptr<ast::syntax::VarDefinition>& dst,const std::shared_ptr<ast::syntax::Expression> &srcExp, size_t dim);
        // copy of the context
        std::shared_ptr<Context> _context;
        // copy of the diagnostic info collector
        std::shared_ptr<diagnostic::DiagnosticStream> _diagnosticStream;

        std::unordered_map<std::string, std::stack<std::shared_ptr<ast::syntax::VarDefinition>>> _variables;
        std::shared_ptr<ast::syntax::Function> _currFunc;
        std::vector<std::string> _layerNames;
        std::string _blockName;
        std::size_t _blockId = 0;
        std::size_t _arrLayer = 0;
    };
}