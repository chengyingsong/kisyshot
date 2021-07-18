#pragma once
#include <diagnostics/diagnostic.h>
#include <context_manager.h>
#include <vector>
#include <memory>

namespace kisyshot::diagnostics{
    class DiagnosticStream{
    public:
        explicit DiagnosticStream(const std::shared_ptr<ContextManager> &contextManager);
        DiagnosticStream& operator<<(std::unique_ptr<Diagnostic> diagPtr);
        friend std::ostream &operator<<(std::ostream& ostream,const DiagnosticStream& diagnosticStream);
        void clear();
    private:
        std::shared_ptr<ContextManager> _contextManager;
        std::vector<std::unique_ptr<Diagnostic>> _diagnostics;
    };
    std::shared_ptr<DiagnosticStream> operator<<(std::shared_ptr<DiagnosticStream>&streamPtr
            ,std::unique_ptr<Diagnostic> diagPtr);
}