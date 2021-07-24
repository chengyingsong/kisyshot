#pragma once
#include <diagnostic/diagnostic.h>
#include <vector>
#include <memory>

namespace kisyshot::diagnostic {
    class DiagnosticStream {
    public:
        DiagnosticStream &operator<<(Diagnostic diagPtr);
        friend std::ostream &operator<<(std::ostream &ostream, const DiagnosticStream &diagnosticStream);
        void clear();

        std::vector<Diagnostic> diagnostics;
    };

    std::shared_ptr<DiagnosticStream>&
    operator<<(std::shared_ptr<DiagnosticStream> &streamPtr, Diagnostic diagPtr);
}