#ifndef ARMS_H
#define ARMS_H

#include "tac.h"
#include "cfg.h"

namespace kisyshot::ast {
    class Arms {
    private:
        typedef enum {
            r0, r1, r2, r3, r4, r5, r6, r7,
            r8, r9, r10, r11, r12, sp, lr, pc
        } Register;

        struct RegContents {
            Var * var;
            std::string name;
            bool isDirty;
            bool mutexLock;
            bool canDiscard;
        } regs[16];

        Register rs, rt, rd;
        // 将寄存器映射至变量
        std::map<Register, Var *> regDescriptor;

        bool varsAreSame(Var * var1, Var * var2);
        int findRegForVar(Var * var);
        int findCleanReg();
        int pickRegForVar(Var * var);
        void discardVarInReg(Var * var, Register reg);

    public:
        Arms();
        std::string generate(const char * fmt, ...);
        void generateDiscardVar(Var * var);
    };
}



#endif