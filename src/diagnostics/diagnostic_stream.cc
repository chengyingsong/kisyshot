#include <diagnostics/diagnostic_stream.h>
namespace kisyshot::diagnostics {
    DiagnosticStream &DiagnosticStream::operator<<(std::unique_ptr<Diagnostic> diagPtr) {
        diagnostic.push_back(std::move(diagPtr));
        return *this;
    }

    std::ostream &operator<<(std::ostream &ostream, const DiagnosticStream &diagnosticStream) {
        for (const auto & _diagnostic : diagnosticStream.diagnostic) {
            _diagnostic->writeTo(ostream);
        }
        return ostream;
    }

    void DiagnosticStream::clear() {
        diagnostic.clear();
    }

    std::shared_ptr<DiagnosticStream>
    operator<<(std::shared_ptr<DiagnosticStream> &streamPtr, std::unique_ptr<Diagnostic> diagPtr) {
        *streamPtr << std::move(diagPtr);
        return streamPtr;
    }
}