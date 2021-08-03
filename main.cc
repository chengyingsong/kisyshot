#include <iostream>
#include <memory>

#include "ast/syntax/syntax_ostream_writer.h"
#include "context_manager.h"
#include "ast/cfg.h"
#include "ast/arms.h"
#include "compiler/armcode.h"

using namespace kisyshot::ast;

int main() {
    std::string_view testCode = R"(int n;
int bubblesort(int arr[])
{
    int i;
    int j;
    i =0;
    while(i < n-1){
    // Last i elements are already in place
        j = 0;
        while(j < n-i-1){
            if (arr[j] > arr[j+1]) {
                // swap(&arr[j], &arr[j+1]);
                int tmp;
                tmp = arr[j+1];
                arr[j+1] = arr[j];
                arr[j] = tmp;
            }
            j = j + 1;
        }
        i = i + 1;
    }
    return 0;
}

int main(){
    n = 10;
    int a[10];
    a[0]=4;a[1]=3;a[2]=9;a[3]=2;a[4]=0;
    a[5]=1;a[6]=6;a[7]=5;a[8]=7;a[9]=8;
    int i;
    i = bubblesort(a);
    while (i < n) {
        int tmp;
        tmp = a[i];
        putint(tmp);
        tmp = 10;
        putch(tmp);
        i = i + 1;
    }
    return 0;
}
int getint();
int getch();
int getarray(int a[]);
void putint(int a);
void putch(int a);
void putarray(int n,int a[]);)";
    auto sm = std::make_shared<kisyshot::ContextManager>();
    auto ctx = sm->create(testCode, "/path/to/test.sy"); //源文件path
    sm->lex(ctx->contextID);
    sm->parse(ctx->contextID);
    sm->check(ctx->contextID);
    kisyshot::compiler::CodeGenerator gen;
    ctx->syntaxTree->genCode(gen, nullptr);
    kisyshot::compiler::ArmCodeGenerator armgen(gen.code, ctx);
    armgen.generateArmCode();
    std::cout << (*ctx->syntaxTree);
    return 0;
}
