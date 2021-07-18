#pragma once

#include <diagnostics/compile_errors.h>
#include <diagnostics/diagnostic.h>
#include <cstddef>
#include <string>
namespace kisyshot::diagnostics{

    /**
     * DiagnosticBuilder is a helper class which builds diagnostics by chain calls
     */
    class DiagnosticBuilder{

    public:
        /**
         * DiagnosticBuildContext is a intermediate class which helps single-way specialized chain call
         */
        class DiagnosticBuildContext{
        public:
            /**
             * write error message into the diagnostic
             * @param error should be written
             * @return diagnostic build intermediate info
             */
            DiagnosticBuildContext& message(const std::string &message);
            /**
             * write suggestion message into the diagnostic
             * @param  suggestion should be written
             * @return diagnostic build intermediate info
             */
            DiagnosticBuildContext& suggestion(const std::string &suggestion);
            virtual std::unique_ptr<Diagnostic> build() = 0;
        protected:
            std::string _message;
            std::string _suggestion;
            std::shared_ptr<Context> _context;
        };

        /**
         * ErrorAtDiagnosticBuildContext is a intermediate class which helps single-way specialized chain call
         */
        class ErrorAtDiagnosticBuildContext : public DiagnosticBuildContext{
        public:
            ErrorAtDiagnosticBuildContext(CompileError error, const std::shared_ptr<Context> &context, std::size_t tokenID);
            /**
             * Builds the diagnostic immediately
             * @return diagnostic built by the DiagnosticBuilder
             */
            std::unique_ptr<Diagnostic> build() override ;

        private:
            CompileError _errorCode;
            std::size_t _tokenID;
        };

        /**
         * ErrorAfterDiagnosticBuildContext is a intermediate class which helps single-way specialized chain call
         */
        class ErrorAfterDiagnosticBuildContext: public DiagnosticBuildContext{
        public:
            ErrorAfterDiagnosticBuildContext(CompileError error, const std::shared_ptr<Context> &context, std::size_t tokenID);
            /**
             * Builds the diagnostic immediately
             * @return diagnostic built by the DiagnosticBuilder
             */
            std::unique_ptr<Diagnostic> build() override ;
            CompileError _errorCode;
            std::size_t _tokenID;
        };

        /**
         * ErrorBeforeDiagnosticBuildContext is a intermediate class which helps single-way specialized chain call
         */
        class ErrorBeforeDiagnosticBuildContext: public DiagnosticBuildContext{
        public:
            ErrorBeforeDiagnosticBuildContext(CompileError error, const std::shared_ptr<Context> &context, std::size_t tokenID);
            /**
             * Builds the diagnostic immediately
             * @return diagnostic built by the DiagnosticBuilder
             */
            std::unique_ptr<Diagnostic> build() override ;
            CompileError _errorCode;
            std::size_t _tokenID;
        };

        /**
         * ErrorRangesDiagnosticBuildContext is a intermediate class which helps single-way specialized chain call
         */
        class ErrorRangesDiagnosticBuildContext: public DiagnosticBuildContext{
            /**
             * Builds the diagnostic immediately
             * @return diagnostic built by the DiagnosticBuilder
             */
            std::unique_ptr<Diagnostic> build() override ;
        };

        /**
         * ErrorBuildContextBuildContext is a intermediate class which helps single-way specialized chain call
         */
        class ErrorBuildContextBuildContext{
        public:
            friend DiagnosticBuilder;
            /**
             * Builds a diagnostic which reminds the programmer that the error occurs AT the specified token index
             * @param tokenID error position
             * @return diagnostic build intermediate info
             */
            ErrorAtDiagnosticBuildContext at(std::size_t tokenID);
            /**
             * Builds a diagnostic which reminds the programmer that the error occurs AFTER the specified token index
             * @param tokenID error position
             * @return diagnostic build intermediate info
             */
            ErrorAfterDiagnosticBuildContext after(std::size_t tokenID);
            /**
             * Builds a diagnostic which reminds the programmer that the error occurs BEFORE the specified token index
             * @param tokenID error position
             * @return diagnostic build intermediate info
             */
            ErrorBeforeDiagnosticBuildContext before(std::size_t tokenID);
             /**
              * Builds a diagnostic which reminds the programmer that the error occurs between specified token indexes
              * @param startTokID
              * @param endTokID
              * @return diagnostic build intermediate info
              */
            ErrorRangesDiagnosticBuildContext ranges(std::size_t startTokID, std::size_t endTokID);
        private:
            ErrorBuildContextBuildContext(CompileError error, const std::shared_ptr<Context> &context);
            CompileError _errorCode;
            std::shared_ptr<Context> _context;
        };

        /**
         * Creates a builder that will build a error diagnostic
         * @param error
         * @param context
         * @return
         */
        static ErrorBuildContextBuildContext error(CompileError error, const std::shared_ptr<Context> &context);
    };
}