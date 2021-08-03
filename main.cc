#include <iostream>
#include <memory>

#include "ast/syntax/syntax_ostream_writer.h"
#include "context_manager.h"
#include "ast/cfg.h"
#include "ast/arms.h"
#include "compiler/armcode.h"

using namespace kisyshot::ast;

int main() {

    std::string_view code = R"(const int base = 16;

int getMaxNum(int n, int arr[]){
    int ret = 0;
    int i = 0888;
    while (i < n){
        if (arr[i] > ret) ret = arr[i];
        i = i + 1;
    }
    return ret;
}

int getNumPos(int num, int pos){
    int tmp = 1;
    int i = 0;
    while (i < pos){
        num = num / base;
        i = i + 1;
    }
    return num % base;
}

<<<<<<< HEAD
    int main(){
        int a[1][2][3] = {1,2,3,4,5,6};
        int t = a[1];
        int x = 1,y = 0;
        int z = 1;
        z = a[x][y][z];
        if(1) {
             y = x % 2;
=======
void radixSort(int bitround, int a[], int l, int r){
    int head[base] = {};
    int tail[base] = {};
    int cnt[base] = {};

    if (bitround == -1 || l + 1 >= r) return;

    {
        int i = l;

        while (i < r){
            cnt[getNumPos(a[i], bitround)]
                = cnt[getNumPos(a[i], bitround)] + 1;
            i = i + 1;
>>>>>>> dev_parser
        }
        head[0] = l;
        tail[0] = l + cnt[0];

        i = 1;
        while (i < base){
            head[i] = tail[i - 1];
            tail[i] = head[i] + cnt[i];
            i = i + 1;
        }
        i = 0;
        while (i < base){
            while (head[i] < tail[i]){
                int v = a[head[i]];
                while (getNumPos(v, bitround) != i){
                    int t = v;
                    v = a[head[getNumPos(t, bitround)]];
                    a[head[getNumPos(t, bitround)]] = t;
                    head[getNumPos(t, bitround)] = head[getNumPos(t, bitround)] + 1;
                }
                a[head[i]] = v;
                head[i] = head[i] + 1;
            }
            i = i + 1;
        }
    }

    {
        int i = l;

        head[0] = l;
        tail[0] = l + cnt[0];

        i = 0;
        while (i < base){
            if (i > 0){
                head[i] = tail[i - 1];
                tail[i] = head[i] + cnt[i];
            }
            radixSort(bitround - 1, a, head[i], tail[i]);
            i = i + 1;
        }
    }

    return;
}


int a[30000010];
int ans;

int main(){
    int n = getarray(a);

    starttime();

    radixSort(8, a, 0, n);

    int i = 0;
    while (i < n){
        ans = ans + i * (a[i] % (2 + i));
        i = i + 1;
    }

    if (ans < 0)
        ans = -ans;
    stoptime();
    putint(ans);
    putch(10);
    return 0;
}
)";

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

    ctx->syntaxTree->genCode(gen, nullptr);

    std::cout << *(ctx->syntaxTree);
    std::cout << *(sm->diagnosticStream);
    ctx->syntaxTree->genCode(gen, nullptr);

    kisyshot::compiler::ArmCodeGenerator armgen(gen.code, ctx);
    armgen.generateArmCode();

    return 0;
}
