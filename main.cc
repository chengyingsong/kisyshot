#include <iostream>
#include <memory>

#include "ast/syntax/syntax_ostream_writer.h"
#include "context_manager.h"
using namespace kisyshot::ast;

int main() {
    std::string_view testCode = R"(int square(int x){
       return x * x;
    }

    void fun(){
    }
    int arr[2][test] = {1,2,3};
    const int test = 2;

    int main(){
        int  a[1][2][3] = {1,2,3,4,5,6};
        int x = 1,y = 0;
        int z = 1;
        arr[0] = 1;
        a[x][y][z] = 1;
        if(1) {
             y = x % 2;
        }

        while(y < 10){
          y = y + 1;
          continue;
          x = x + 1;
        }

        z =  y + square(x);
        fun();
        put("string test");
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
