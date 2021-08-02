#include <iostream>
#include <memory>

#include "ast/syntax/syntax_ostream_writer.h"
#include "context_manager.h"
#include "ast/cfg.h"
#include "ast/arms.h"
#include "compiler/armcode.h"

using namespace kisyshot::ast;

int main() {
//TODO:  数组还有字符串，变量名有下划线开头的，把temp改成@temp。
    std::string_view testCode = R"(int square(int x){
       return x * x;
    }

    void fun(){
    }

    int main(){
        int a[1][2][3] = {1,2,3,4,5,6};
        int t = a[1];
        int x = 1,y = 0;
        int z = 1;
        z = a[x][y][z];
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

    kisyshot::compiler::ArmCodeGenerator armgen(gen.code, ctx);
    armgen.generateArmCode();

    return 0;
}
