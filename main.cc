#include <iostream>
#include <memory>

#include "ast/syntax/syntax_ostream_writer.h"
#include "context_manager.h"
#include "ast/cfg.h"

using namespace kisyshot::ast;

int main() {
//TODO:  单目运算符，数组，break和continue还有字符串
    std::string_view testCode = R"( int square(int x){
        return x * x;
    }
    const int k = 4;
    int main(){
        int x = 1,y = 3;
        int z = x + y * 3;
        if( z == 0) {
             y = x % 2;
        }
        int i = 0;
        while(i < 10){
           i = i + 1;
         }
         y = square(y) + 5 % 2;
        return 0;
    })";

    auto sm = std::make_shared<kisyshot::ContextManager>();
    auto ctx = sm->create(testCode, "/path/to/test.sy");
    sm->lex(ctx->contextID);
    sm->parse(ctx->contextID);
    kisyshot::compiler::CodeGenerator gen;
    ctx->syntaxTree->genCode(gen, nullptr);
    std::cout << *(ctx->syntaxTree);
    std::cout << *(sm->diagnosticStream);
    return 0;
}
