#include "context_manager.h"

#include <compiler/lexer.h>
#include <compiler/parser.h>
#include <fstream>
namespace kisyshot{
    std::shared_ptr<Context> ContextManager::create(const std::string_view& code, const std::string & path) {
        auto context = std::make_shared<Context>(code, _contexts.size());
        context->path = path;
        _contexts.push_back(context);
        return context;
    }
    std::shared_ptr<Context> ContextManager::access(std::size_t index) {
        if(index < _contexts.size())
            return _contexts[index];
        return nullptr;
    }

    std::shared_ptr<Context> ContextManager::operator[](std::size_t index) {
        if(index < _contexts.size())
            return _contexts[index];
        return nullptr;
    }

    ContextManager::ContextManager(): diagnosticStream(std::make_shared<diagnostic::DiagnosticStream>()) {

    }

    bool ContextManager::lex(std::size_t index) {
        return compiler::Lexer(_contexts[index], diagnosticStream).lex();
    }

    void ContextManager::parse(std::size_t index) {
        compiler::Parser(_contexts[index],diagnosticStream).parse();
    }

    std::shared_ptr<Context> ContextManager::load(const std::string &path) {
        std::string s;
        std::ifstream in(path, std::ios::in | std::ios::binary);
        if (in) {
            in.seekg(0, std::ios::end);
            s.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&s[0], long(s.size()));
            in.close();
        }

        auto ctx = create(s, path);
        _ctxCodes[ctx->contextID] = std::move(s);
        return ctx;
    }
}