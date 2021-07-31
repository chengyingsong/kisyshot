#pragma once

#include <diagnostic/diagnostic_stream.h>
#include "context.h"
namespace kisyshot {
    /**
     * A Context manager: which is going to be used in multiple file compilations.
     */
    class ContextManager {
    public:
        /**
         * Constructor of ContextManager, which will initialize diagnostic stream.
         */
        ContextManager();

        /**
         * Create a SyntaxContext by given code reference string and path reference string.
         * @param code
         * @return
         */
        std::shared_ptr<Context> create(const std::string_view &code, const std::string& path);

        /**
         * Create a Context by loading file from path
         * @param path
         * @return
         */
        std::shared_ptr<Context> load(const std::string& path);

        /**
         * Access to the SyntaxContext with the given index
         * @param index
         * @return
         */
        std::shared_ptr<Context> access(std::size_t index);

        /**
         * Access to the Context with the given index
         * @param index
         * @return
         */
        std::shared_ptr<Context> operator[](std::size_t index);

        /**
         * Lex the given Context's code determined by the given index
         * @param index
         */
        bool lex(std::size_t index);

        /**
         * Parse the given Context's code determined by the given index
         * @param index
         */
        void parse(std::size_t index);

        /**
         * Semantically analyse the syntax tree, do typeck && generates symbol table
         * @param index
         */
        void check(std::size_t index);

        /**
         * Diagnostic stream of the context manager, which provides diagnostic service for the compiler.
         */
        std::shared_ptr<diagnostic::DiagnosticStream> diagnosticStream;
    private:
        std::vector<std::shared_ptr<Context>> _contexts;
        std::unordered_map<std::size_t, std::string> _ctxCodes;
    };
}