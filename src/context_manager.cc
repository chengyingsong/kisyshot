#include "context_manager.h"

#include <compiler/lexer.h>
#include <compiler/parser.h>
#include <compiler/sema.h>
#include <fstream>
namespace kisyshot{
    std::shared_ptr<Context> ContextManager::create(const std::string& code, const std::string & path) {
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

    void ContextManager::check(std::size_t index){
        compiler::Sema(_contexts[index],diagnosticStream).check();
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
        s += "int getint();\n"
             "int getch();\n"
             "int getarray(int a[]);\n"
             "void putint(int a);\n"
             "void putch(int a);\n"
             "void putarray(int n,int a[]);\n"
             "void _sysy_starttime(int lineno);\n"
             "void _sysy_stoptime(int lineno);\n"
             "int __aeabi_idivmod(int a, int b);\n";
        auto ctx = create(s, path);
        return ctx;
    }
}