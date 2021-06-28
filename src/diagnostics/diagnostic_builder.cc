#include <diagnostics/diagnostic_builder.h>

namespace kisyshot::diagnostics {
    DiagnosticBuilder::DiagnosticBuildContext &
    DiagnosticBuilder::DiagnosticBuildContext::message(const std::string &message) {
        _message = message;
        return *this;
    }

    DiagnosticBuilder::DiagnosticBuildContext &
    DiagnosticBuilder::DiagnosticBuildContext::suggestion(const std::string &suggestion) {
        _suggestion = suggestion;
        return *this;
    }

    std::unique_ptr<Diagnostic> DiagnosticBuilder::ErrorAtBuildContext::build() {
        return std::make_unique<ErrorAtDiagnostic>(_errorCode, _contextID, _tokenID, _message, _suggestion);
    }

    DiagnosticBuilder::ErrorAtBuildContext::ErrorAtBuildContext(CompileError error, std::size_t contextID,
                                                                std::size_t tokenID) {
        _errorCode = error;
        _contextID = contextID;
        _tokenID = tokenID;
    }

    DiagnosticBuilder::ErrorAfterBuildContext::ErrorAfterBuildContext(CompileError error, std::size_t contextID,
                                                                      std::size_t tokenID) {
        _errorCode = error;
        _contextID = contextID;
        _tokenID = tokenID;
    }

    std::unique_ptr<Diagnostic> DiagnosticBuilder::ErrorAfterBuildContext::build() {
        return std::make_unique<ErrorAfterDiagnostic>(_errorCode, _contextID, _tokenID, _message, _suggestion);
    }

    DiagnosticBuilder::ErrorBeforeBuildContext::ErrorBeforeBuildContext(CompileError error, std::size_t contextID,
                                                                        std::size_t tokenID) {
        _errorCode = error;
        _contextID = contextID;
        _tokenID = tokenID;
    }

    std::unique_ptr<Diagnostic> DiagnosticBuilder::ErrorBeforeBuildContext::build() {
        return std::make_unique<ErrorBeforeDiagnostic>(_errorCode, _contextID, _tokenID, _message, _suggestion);
    }

    DiagnosticBuilder::ErrorBuildContextBuildContext::ErrorBuildContextBuildContext(CompileError error,
                                                                                    std::size_t contextID) {
        _errorCode = error;
        _contextID = contextID;
    }

    DiagnosticBuilder::ErrorAtBuildContext DiagnosticBuilder::ErrorBuildContextBuildContext::at(std::size_t tokenID) {
        return ErrorAtBuildContext(_errorCode, _contextID, tokenID);
    }

    DiagnosticBuilder::ErrorAfterBuildContext
    DiagnosticBuilder::ErrorBuildContextBuildContext::after(std::size_t tokenID) {
        return ErrorAfterBuildContext(_errorCode, _contextID, tokenID);
    }

    DiagnosticBuilder::ErrorBeforeBuildContext
    DiagnosticBuilder::ErrorBuildContextBuildContext::before(std::size_t tokenID) {
        return ErrorBeforeBuildContext(_errorCode, _contextID, tokenID);
    }


    DiagnosticBuilder::ErrorBuildContextBuildContext
    DiagnosticBuilder::error(CompileError error, std::size_t contextID) {
        return ErrorBuildContextBuildContext(error, contextID);
    }
}