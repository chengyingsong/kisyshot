#pragma once
#include <memory>
#include "context.h"
#include "ast/token.h"
namespace kisyshot::compiler{
    /**
     * Lexical analyser for SysY language
     */
    class lexer {
    public:
        /**
         * Constructs a lexer from given syntax analyse context.
         *
         * The life of the raw codes of the tokens lexed by this lexer is decided by the life time of the life time of
         * member 'code' of the given parameter 'context'. Result of accessing to context's tokens is undefined after
         * the content of the code have been collected.
         * @param context:  the compile context during lexical analyse and syntax analyse
         * @param diagnosticStream: the diagnostic info collector to report errors to
         */
        explicit lexer(const std::shared_ptr<context> &context);

        /**
         * Lex all tokens from given code string and store all into context.
         *
         * This will directly write all tokens into context.
         */
        bool lex();
        // get the token_type name
        std::string get_token_type_name(int token_type_code);
    private:
        // copy of the code
        std::string_view _code;
        // syntax context
        std::shared_ptr<context> _context;
        // current relative position to the first char in the _code variable
        size_t _position;
        // marks if the lexer had met the end of the code file
        bool _eof;
        // lex a token and write it into context
        bool next();
        // continue to lex a numeric const
        bool next_numeric_literal();
        // continue to lex a identifier
        bool next_identifier(size_t offset = 0);
        // continue to lex a operator
        bool next_operator();
        // continue to lex a inline comment
        bool next_inline_comment();
        // continue to lex a interline comment
        bool next_interline_comment();
        bool is_splitter();

        ast::token_type_t curr_token_type();
        bool curr_token_is(ast::token_type_t token_type);
    };
}
