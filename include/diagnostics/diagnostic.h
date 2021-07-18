#pragma once

#include <memory>
#include <ostream>
#include <string>
#include <vector>
#include <diagnostics/compile_errors.h>
#include "context.h"
#include "context_manager.h"

namespace kisyshot::diagnostics{
    /**
     * Abstract class that defines a diagnostic should implements.
     */
    class Diagnostic{
    public:
        virtual void writeTo(std::ostream& stream,const std::shared_ptr<ContextManager>& ctxManager) = 0;
        virtual ~Diagnostic() = default;
    };

    /**
     * A diagnostic that issues error message at the position of the specified Token.
     */
    class ErrorAtDiagnostic: public Diagnostic{
    public:
        ErrorAtDiagnostic(CompileError error, std::size_t contextID, std::size_t tokenID,
                          const std::string& message,const std::string& suggestion);
        void writeTo(std::ostream& stream,const std::shared_ptr<ContextManager>& ctxManager) override ;
    private:
        CompileError _errorCode;
        std::size_t _contextID;
        std::size_t _tokenID;
        std::string _message;
        std::string _suggestion;
    };

    /**
     * A diagnostic that issues error message after the position of the specified Token.
     */
    class ErrorAfterDiagnostic: public Diagnostic{
    public:
        ErrorAfterDiagnostic(CompileError error, std::size_t contextID, std::size_t tokenID,
                             const std::string& message,const std::string& suggestion);
        void writeTo(std::ostream& stream,const std::shared_ptr<ContextManager>& ctxManager) override ;
    private:
        CompileError _errorCode;
        std::size_t _contextID;
        std::size_t _tokenID;
        std::string _message;
        std::string _suggestion;
    };

    /**
     * A diagnostic that issues error message before the position of the specified Token.
     */
    class ErrorBeforeDiagnostic: public Diagnostic{
    public:
        ErrorBeforeDiagnostic(CompileError error, std::size_t contextID, std::size_t tokenID,
                             const std::string& message,const std::string& suggestion);
        void writeTo(std::ostream& stream,const std::shared_ptr<ContextManager>& ctxManager) override ;
    private:
        CompileError _errorCode;
        std::size_t _contextID;
        std::size_t _tokenID;
        std::string _message;
        std::string _suggestion;
    };

    /**
     * A diagnostic that issues error message between the tokens in the range
     */
    class ErrorRangesDiagnostic: public Diagnostic{

    };
}