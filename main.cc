#include <iostream>
#include <memory>

#include "ast/syntax/syntax_ostream_writer.h"
#include "context_manager.h"


using namespace kisyshot::ast;

int main() {
    std::string_view testCode = R"( int main(){
        int x = 1,k;
        int y = 3;
        int z = x + y * 3;
        if( z == 0) {
             y = x % 2;
        }
        int i = 0;
        while(i < 10){
           i = i + 1;
         }
         y = square(y);
        return 0;
})";

    auto sm = std::make_shared<kisyshot::ContextManager>();
    auto ctx = sm->create(testCode, "/path/to/test.sy");
    kisyshot::compiler::CodeGenerator gen;

    sm->lex(ctx->contextID);
    sm->parse(ctx->contextID);
    sm->check(ctx->contextID);
//TODO: 数组左值应该是Store指令
    ctx->syntaxTree->genCode(gen, nullptr);

    std::cout << *(ctx->syntaxTree);
    std::cout << *(sm->diagnosticStream);
    return 0;
}
