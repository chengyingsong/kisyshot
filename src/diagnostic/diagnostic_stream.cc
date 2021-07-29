#include <diagnostic/diagnostic_stream.h>
namespace kisyshot::diagnostic {
    DiagnosticStream &DiagnosticStream::operator<<(Diagnostic d) {
        diagnostics.push_back(std::move(d));
        return *this;
    }

    std::ostream &operator<<(std::ostream &ostream, const DiagnosticStream &diagnosticStream) {
        for (const auto & d : diagnosticStream.diagnostics) {
            d.writeTo(ostream);
        }
        return ostream;
    }

    void DiagnosticStream::clear() {
        diagnostics.clear();
    }

    std::shared_ptr<DiagnosticStream>&
    operator<<(std::shared_ptr<DiagnosticStream> &streamPtr, Diagnostic diagnostic) {
        *streamPtr << std::move(diagnostic);
        return streamPtr;
    }
}