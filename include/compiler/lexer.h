#pragma once

#include <ast/token.h>
#include <memory>
#include <diagnostic/diagnostic_stream.h>
#include "context.h"

namespace kisyshot::compiler{
    /**
     * Lexical analyser for SysY language
     */
    class Lexer {
    public:
        /**
         * Constructs a Lexer from given syntax analyse Context.
         *
         * The life of the raw codes of the tokens lexed by this Lexer is decided by the life time of the life time of
         * member 'code' of the given parameter 'Context'. Result of accessing to Context's tokens is undefined after
         * the content of the code have been collected.
         * @param context:  the compile Context during lexical analyse and syntax analyse
         * @param diagnosticStream: the diagnostic info collector to report errors to
         */
        explicit Lexer(const std::shared_ptr<Context> &context,
                       const std::shared_ptr<diagnostic::DiagnosticStream> &diagnosticStream);

        /**
         * Lex all tokens from given code string and store all into Context.
         *
         * This will directly write all tokens into Context.
         */
        bool lex();
    private:
        // copy of the code
        std::string_view _code;
        // syntax Context
        std::shared_ptr<Context> _context;
        // diagnostic strean=m
        std::shared_ptr<diagnostic::DiagnosticStream> _diagnosticStream;
        // current relative position to the first char in the _code variable
        size_t _position;
        // marks if the Lexer had met the end of the code file
        bool _eof;
        // lex a Token and write it into Context
        bool next();
        // continue to lex a numeric const
        bool nextNumericLiteral();
        // continue to lex a identifier
        bool nextIdentifier(size_t offset = 0);
        // continue to lex a operator
        bool nextOperator();
        // continue to lex a inline comment
        bool nextInlineComment();
        // continue to lex a interline comment
        bool nextInterlineComment();
        bool isSplitter();

        ast::TokenType currTokenType();
        bool currTokenIs(ast::TokenType token_type);
    };
}
