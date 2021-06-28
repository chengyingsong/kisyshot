#pragma once

#include <memory>
#include <list>
#include <set>

#include <ast/operator.h>
#include <ast/syntax/function.h>
#include <ast/token.h>
#include <ast/syntax/var_declaration.h>
#include <diagnostics/diagnostic_stream.h>
#include "context.h"

namespace kisyshot::compiler {
    /**
     * Syntax analyser for SysY language. [WIP]
     */
    class Parser {
    public:
        /**
         * Constructs a lexer from given syntax analyse context and diagnostic info reporter.
         * @param context:  the compile context during lexical analyse and syntax analyse
         * @param diagnosticStream: the diagnostic info collector to report errors to
         */
        Parser(const std::shared_ptr<Context> &context,
               const std::shared_ptr<diagnostics::DiagnosticStream> &diagStream);

        void parse();

        std::shared_ptr<ast::syntax::Identifier> parseIdentifier();

        std::shared_ptr<ast::syntax::Function> parseFunction();

        std::shared_ptr<ast::syntax::Expression> parseExpression(const std::set<ast::TokenType> &endTokens = {},
                                                                 ast::OperatorPrecedence parentPrecedence = ast::OperatorPrecedence::initial);

        std::shared_ptr<ast::syntax::Statement> parseStatement();

        std::shared_ptr<ast::syntax::VarDeclaration> parseVariableDeclaration();

        std::shared_ptr<ast::syntax::BlockStatement> parseBlockStatement();

        std::shared_ptr<ast::syntax::IfStatement> parseIfStatement();

        std::shared_ptr<ast::syntax::WhileStatement> parseWhileStatement();


    private:
        /**
         * Move the given token position to the next non-comment token's position
         * @param outPos reference to the pos to be moved
         * @return true will be returned if move action finished successfully, false will be returned when there's no
         * next non-comment token.
         */
        bool move(size_t &outPos);

        /**
         * Step `current` position to next non-comment token and then recoverPosition the `lookahead` token position
         * @return true will be returned if step action finished successfully, false will be returned when there's no
         * next non-comment token.
         */
        bool step();

        /**
         * Set `lookahead` position to the move() action result of `current`
         * @return true will be returned if step action finished successfully, false will be returned when there's no
         * next non-comment token.
         */
        bool recoverPosition();

        ast::TokenType current();

        ast::TokenType lookahead();

        // copy of the context
        std::shared_ptr<Context> _context;
        // copy of the diagnostic info collector
        std::shared_ptr<diagnostics::DiagnosticStream> _diagnosticStream;
        // the begin position of current parsing syntax
        size_t _current;
        // the lookahead position to
        size_t _lookahead;
    };
}
