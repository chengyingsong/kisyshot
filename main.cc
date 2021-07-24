#include <iostream>
#include <memory>

#include "ast/syntax/syntax_ostream_writer.h"
#include "context_manager.h"
using namespace kisyshot::ast;

int main() {
//TODO: 先处理函数声明，变量声明和表达式支持（暂时不管数组和字符串
//TODO： 变量声明，转换成Load指令，Load指令应该返回temp类型，表达式应该也返回temp类型
//TODO: 先清理其他的代码，完成代码生成的类，实现Unit，Function和Expression的代码。


    /*
     * .main:
     * BeginFunc
     * _temp1 = y * 3
     * _temp2 = x + _temp1
     * z = _temp2
     * return 0
     * EndFunc
     * */
    std::string_view testCode = R"( int main(){
        int x = 1,k;
        int y = 3;
        int z = x + y * 3;
        if( z == 0) {
             y = x % 2;
        }
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
