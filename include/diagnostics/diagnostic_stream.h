#pragma once
#include <diagnostics/diagnostic.h>
#include <vector>
#include <memory>

namespace kisyshot::diagnostics {
    class DiagnosticStream {
    public:
        DiagnosticStream &operator<<(std::unique_ptr<Diagnostic> diagPtr);
        friend std::ostream &operator<<(std::ostream &ostream, const DiagnosticStream &diagnosticStream);
        void clear();

    private:
        std::vector<std::unique_ptr<Diagnostic>> _diagnostics;
    };

    std::shared_ptr<DiagnosticStream>
    operator<<(std::shared_ptr<DiagnosticStream> &streamPtr, std::unique_ptr<Diagnostic> diagPtr);
}