#include <diagnostics/diagnostic_stream.h>
namespace kisyshot::diagnostics {

    DiagnosticStream::DiagnosticStream(const std::shared_ptr<ContextManager> &contextManager) {
        _contextManager = contextManager;
    }

    DiagnosticStream &DiagnosticStream::operator<<(std::unique_ptr<Diagnostic> diagPtr) {
        _diagnostics.push_back(std::move(diagPtr));
        return *this;
    }

    std::ostream &operator<<(std::ostream &ostream, const DiagnosticStream &diagnosticStream) {
        for (std::size_t i = 0; i < diagnosticStream._diagnostics.size(); ++i) {
            diagnosticStream._diagnostics[i]->writeTo(ostream, diagnosticStream._contextManager);
        }
        return ostream;
    }

    void DiagnosticStream::clear() {
        _diagnostics.clear();
    }

    std::shared_ptr<DiagnosticStream>
    operator<<(std::shared_ptr<DiagnosticStream> &streamPtr, std::unique_ptr<Diagnostic> diagPtr) {
        *streamPtr << std::move(diagPtr);
        return streamPtr;
    }
}