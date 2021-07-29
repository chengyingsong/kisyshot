#include <iostream>
#include <memory>

#include "ast/syntax/syntax_ostream_writer.h"
#include "context_manager.h"
using namespace kisyshot::ast;

int main() {
//TODO:  数组，break和continue还有字符串
    std::string_view testCode = R"(
    int main(){
        int x = 1,y = 3;
        int z = 0;
        if(1) {
             y = x % 2;
        }
        return 0;
    })";
//TODO: Var Definetion
    auto sm = std::make_shared<kisyshot::ContextManager>();
    auto ctx = sm->create(testCode, "/path/to/test.sy");
    sm->lex(ctx->contextID);
    sm->parse(ctx->contextID);
    sm->check(ctx->contextID);
    kisyshot::compiler::CodeGenerator gen;
    ctx->syntaxTree->genCode(gen, nullptr);
    std::cout << *(ctx->syntaxTree);
    std::cout << *(sm->diagnosticStream);
    return 0;
}
