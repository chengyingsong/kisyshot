#pragma once
#include <ast/operator.h>
#include <ast/syntax/function.h>
#include <ast/token.h>
#include <ast/syntax/var_declaration.h>
#include <diagnostics/diagnostic_stream.h>
#include <set>
#include <stack>
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

        /**
         * Parse an identifier starts from current position
         * @return identifier parsed
         */
        std::shared_ptr<ast::syntax::Identifier> parseIdentifier();

        /**
         * Parse a function definition like int function(int var){...statements...} starts from current position
         * @return function parsed
         */
        std::shared_ptr<ast::syntax::Function> parseFunction();

        /**
         * Parse an expression expression starts from current position
         * @return expression parsed
         */
        std::shared_ptr<ast::syntax::Expression> parseExpression(const std::set<ast::TokenType> &endTokens = {},
                                                                 ast::OperatorPrecedence parentPrecedence = ast::OperatorPrecedence::initial);

        /**
         * Parse a type definition starts from current position
         * @return type parsed
         */
        std::shared_ptr<ast::syntax::Type> parseType();

        /**
         * Parse a statement definition starts from current position
         * @return statement parsed
         */
        std::shared_ptr<ast::syntax::Statement> parseStatement();

        /**
         * Parse a variable declaration starts from current position
         * @return variable declaration parsed
         */
        std::shared_ptr<ast::syntax::VarDeclaration> parseVariableDeclaration();

        /**
         * Parse a block statement starts from current position
         * @return block statement parsed
         */
        std::shared_ptr<ast::syntax::BlockStatement> parseBlockStatement();

        /**
         * Parse a if statement starts from current position
         * @return if statement parsed
         */
        std::shared_ptr<ast::syntax::IfStatement> parseIfStatement();

        /**
         * Parse a while statement starts from current position
         * @return while statement parsed
         */
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
         * Step `current` position to next non-comment token and then call prepareLookahead() to prepare lookahead
         * @return true will be returned if step action finished successfully, false will be returned when there's no
         * next non-comment token.
         */
        bool step();

        /**
         * Set `lookahead` position to the move() action result of `current`
         * @return true will be returned if step action finished successfully, false will be returned when there's no
         * next non-comment token.
         */
        bool prepareLookahead();

        /**
         * Push `current` position into recover token stack.
         */
        void pushRecover();

        /**
         * Push `current` position into recover token stack, and then step() one.
         * @return true will be returned if step action finished successfully, false will be returned when there's no
         * next non-comment token.
         */
        bool pushRecoverAndStep();

        /**
         * Set `current` position to the position popped from recover stack, and call prepareLookahead()
         * Does nothing when no available recover
         */
        void recover();

        /**
         * Get token type determined by '_current' position
         * @return current token type
         */
        ast::TokenType current();

        /**
         * Get token type determined by '_lookahead' position
         * @return lookahead token type
         */
        ast::TokenType lookahead();

        // copy of the context
        std::shared_ptr<Context> _context;
        // copy of the diagnostic info collector
        std::shared_ptr<diagnostics::DiagnosticStream> _diagnosticStream;
        // the begin position of current parsing syntax
        size_t _current;
        // the lookahead position, which can help accelerate parsing
        size_t _lookahead;
        //
        std::stack<std::size_t> _recover;
    };
}
