#pragma GCC diagnostic ignored "-Wpedantic"
#include <ast/arms.h>
#include <stdarg.h>
#include <stdio.h>

using namespace kisyshot::ast;

bool Arms::varsAreSame(Var * var1, Var * var2) {
    return (var1 == var2 || (var1 && var2 && (var1->getName() == var2->getName()) && (var1->getBase() == var2->getBase()) && var1->getOffset() == var2->getOffset()));
}

int Arms::findRegForVar(Var * var) {
    std::map<Register, Var *>::iterator it;
    Register reg = (Register)r0;
    bool flag = false;
    for (it = regDescriptor.begin(); it != regDescriptor.end(); it++)
        if (varsAreSame(var, it->second)) {
            reg = it->first;
            flag = true;
        }

    if (flag)
        return reg;
    else
        return -1;
}

int Arms::findCleanReg() {
    int index = -1;
    for (int i = r0; i <= r12; i++) {
        if (regs[i].isDirty == false && i != r7) {
            index = i;
            break;
        }
    }

    return index;
}

int Arms::pickRegForVar(Var * var) {
    if (findRegForVar(var) != -1)
        return findRegForVar(var);
    else
        return findCleanReg();
}

void Arms::discardVarInReg(Var * var, Register reg) {

}

Arms::Arms() {
    regs[r0] = (RegContents){NULL, "r0", false, false, false};
    regs[r1] = (RegContents){NULL, "r1", false, false, false};
    regs[r2] = (RegContents){NULL, "r2", false, false, false};
    regs[r3] = (RegContents){NULL, "r3", false, false, false};
    regs[r4] = (RegContents){NULL, "r4", false, false, false};
    regs[r5] = (RegContents){NULL, "r5", false, false, false};
    regs[r6] = (RegContents){NULL, "r6", false, false, false};
    regs[r7] = (RegContents){NULL, "r7", false, false, false};
    regs[r8] = (RegContents){NULL, "r8", false, false, false};
    regs[r9] = (RegContents){NULL, "r9", false, false, false};
    regs[r10] = (RegContents){NULL, "r10", false, false, false};
    regs[r11] = (RegContents){NULL, "r11", false, false, false};
    regs[r12] = (RegContents){NULL, "r12", false, false, false};
    regs[sp] = (RegContents){NULL, "sp", false, false, false};
    regs[lr] = (RegContents){NULL, "lr", false, false, false};
    regs[pc] = (RegContents){NULL, "pc", false, false, false};
}

std::string Arms::generate(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buf[1024];
    vsprintf(buf, fmt, args);
    va_end(args);
    std::string code = "";
    if (buf[strlen(buf) - 1] != ':')
        code += "\t";
    code += std::string(buf);
    code += "\n";

    return code;
}

void Arms::generateDiscardVar(Var * var) {
    rd = (Register)pickRegForVar(var);
    regs[rd].canDiscard = true;
}