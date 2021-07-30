#include <iostream>
#include <memory>

#include "ast/syntax/syntax_ostream_writer.h"
#include "context_manager.h"
using namespace kisyshot::ast;

int main() {
//TODO:  数组还有字符串
    std::string_view testCode = R"(int square(int x){
       return x * x;
    }

    int main(){
        int x = 1,y = 3;
        int z = 0;
        if(1) {
             y = x % 2;
        }

        while(y < 10){
          y = y + 1;
          continue;
          x = x + 1;
        }

        z =  y + square(x);
        return 0;
    })";
    auto sm = std::make_shared<kisyshot::ContextManager>();
    auto ctx = sm->create(testCode, "/path/to/test.sy");
    sm->lex(ctx->contextID);
    sm->parse(ctx->contextID);
    sm->check(ctx->contextID);
    kisyshot::compiler::CodeGenerator gen;

    std::cout << *(ctx->syntaxTree);
    std::cout << *(sm->diagnosticStream);
    ctx->syntaxTree->genCode(gen, nullptr);
    return 0;
}
