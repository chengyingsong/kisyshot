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

    std::unique_ptr<Diagnostic> DiagnosticBuilder::ErrorAtDiagnosticBuildContext::build() {
        return std::make_unique<ErrorAtDiagnostic>(_errorCode, _context, _tokenID, _message, _suggestion);
    }

    DiagnosticBuilder::ErrorAtDiagnosticBuildContext::ErrorAtDiagnosticBuildContext(CompileError error, const std::shared_ptr<Context> &context,
                                                                                    std::size_t tokenID) {
        _errorCode = error;
        _context = context;
        _tokenID = tokenID;
    }

    DiagnosticBuilder::ErrorAfterDiagnosticBuildContext::ErrorAfterDiagnosticBuildContext(CompileError error, const std::shared_ptr<Context> &context,
                                                                                          std::size_t tokenID) {
        _errorCode = error;
        _context = context;
        _tokenID = tokenID;
    }

    std::unique_ptr<Diagnostic> DiagnosticBuilder::ErrorAfterDiagnosticBuildContext::build() {
        return std::make_unique<ErrorAfterDiagnostic>(_errorCode, _context, _tokenID, _message, _suggestion);
    }

    DiagnosticBuilder::ErrorBeforeDiagnosticBuildContext::ErrorBeforeDiagnosticBuildContext(CompileError error, const std::shared_ptr<Context> &context,
                                                                                            std::size_t tokenID) {
        _errorCode = error;
        _context = context;
        _tokenID = tokenID;
    }

    std::unique_ptr<Diagnostic> DiagnosticBuilder::ErrorBeforeDiagnosticBuildContext::build() {
        return std::make_unique<ErrorBeforeDiagnostic>(_errorCode, _context, _tokenID, _message, _suggestion);
    }

    DiagnosticBuilder::ErrorBuildContextBuildContext::ErrorBuildContextBuildContext(CompileError error
    , const std::shared_ptr<Context> &context) {
        _errorCode = error;
        _context = context;
    }

    DiagnosticBuilder::ErrorAtDiagnosticBuildContext DiagnosticBuilder::ErrorBuildContextBuildContext::at(std::size_t tokenID) {
        return ErrorAtDiagnosticBuildContext(_errorCode, _context, tokenID);
    }

    DiagnosticBuilder::ErrorAfterDiagnosticBuildContext
    DiagnosticBuilder::ErrorBuildContextBuildContext::after(std::size_t tokenID) {
        return ErrorAfterDiagnosticBuildContext(_errorCode, _context, tokenID);
    }

    DiagnosticBuilder::ErrorBeforeDiagnosticBuildContext
    DiagnosticBuilder::ErrorBuildContextBuildContext::before(std::size_t tokenID) {
        return ErrorBeforeDiagnosticBuildContext(_errorCode, _context, tokenID);
    }


    DiagnosticBuilder::ErrorBuildContextBuildContext
    DiagnosticBuilder::error(CompileError error, const std::shared_ptr<Context> &context) {
        return ErrorBuildContextBuildContext(error, context);
    }
}