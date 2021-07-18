#pragma once

#include <diagnostics/compile_errors.h>
#include <diagnostics/diagnostic.h>
#include <cstddef>
#include <string>
namespace kisyshot::diagnostics{

    class DiagnosticBuilder{

    public:
        class DiagnosticBuildContext{
        public:
            DiagnosticBuildContext& message(const std::string &message);
            DiagnosticBuildContext& suggestion(const std::string &suggestion);
            virtual std::unique_ptr<Diagnostic> build() = 0;
        protected:
            std::string _message;
            std::string _suggestion;
            std::size_t _contextID;
        };

        class ErrorAtBuildContext : public DiagnosticBuildContext{
        public:
            ErrorAtBuildContext(CompileError error, std::size_t contextID, std::size_t tokenID);
            std::unique_ptr<Diagnostic> build() override ;

        private:
            CompileError _errorCode;
            std::size_t _tokenID;
        };

        class ErrorAfterBuildContext: public DiagnosticBuildContext{
        public:
            ErrorAfterBuildContext(CompileError error,std::size_t contextID,std::size_t tokenID);
            std::unique_ptr<Diagnostic> build() override ;
            CompileError _errorCode;
            std::size_t _tokenID;
        };

        class ErrorBeforeBuildContext: public DiagnosticBuildContext{
        public:
            ErrorBeforeBuildContext(CompileError error,std::size_t contextID,std::size_t tokenID);
            std::unique_ptr<Diagnostic> build() override ;
            CompileError _errorCode;
            std::size_t _tokenID;
        };

        class ErrorRangesBuildContext: public DiagnosticBuildContext{
            std::unique_ptr<Diagnostic> build() override ;
        };

        class ErrorBuildContextBuildContext{
        public:
            friend DiagnosticBuilder;
            ErrorAtBuildContext at(std::size_t tokenID);
            ErrorAfterBuildContext after(std::size_t tokenID);
            ErrorBeforeBuildContext before(std::size_t tokenID);
            ErrorRangesBuildContext ranges(std::size_t startTokID,std::size_t endTokID);
        private:
            ErrorBuildContextBuildContext(CompileError error, std::size_t contextID);
            CompileError _errorCode;
            std::size_t _contextID;
        };

        static ErrorBuildContextBuildContext error(CompileError error, std::size_t contextID);

    };
}