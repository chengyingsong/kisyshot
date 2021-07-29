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
    if (index == -1) {
        index = selectRandomReg();
        cleanReg((Register)index);
    }

    return index;
}

int Arms::selectRandomReg() {
    bool flag = false;
    int select = 0;
    while (!flag) {
        select = rand() % 13;
        if (select != r7)
            if (regs[select].mutexLock == false)
                if (regs[select].isDirty)
                    flag = true;
    }

    return select;
}

int Arms::pickRegForVar(Var * var) {
    if (findRegForVar(var) != -1)
        return findRegForVar(var);
    else
        return findCleanReg();
}

Var * Arms::getRegContents(Register reg) {
    std::map<Register, Var *>::iterator it;
    it = regDescriptor.find(reg);
    if (it == regDescriptor.end())
        return (Var *)NULL;
    else
        return it->second;
}

void Arms::cleanReg(Register reg) {
    Var * var = getRegContents(reg);
    if (var == NULL)
        return;
    spillReg(var, reg);
}

void Arms::cleanRegForBranch() {
    for (int i = r0; i <= r12; i++)
        if (regs[i].isDirty == true && i != r7)
            cleanReg((Register)i);
}

std::map<Arms::Register, Var *>::iterator Arms::regDescriptorFind(Var * var) {
    std::map<Register, Var *>::iterator it;
    bool flag = false;
    for (it = regDescriptor.begin(); it != regDescriptor.end(); it++)
        if (varsAreSame(var, it->second)) {
            flag = true;
            break;
        }
    if (flag)
        return it;
    else
        return regDescriptor.end();
}

void Arms::regDescriptorInsert(Var * var, Register reg) {
    if (findRegForVar(var) != -1) {
        regDescriptorUpdate(var);
        return;
    }
    regDescriptor.insert(std::pair<Register, Var *>(reg, var));
    regs[reg].isDirty = true;
}

void Arms::regDescriptorRemove(Var * var, Register reg) {
    if (findRegForVar(var) != -1) {
        regs[reg].isDirty = false;
        std::map<Register, Var *>::iterator it = regDescriptorFind(var);
        regDescriptor.erase(it);
    }
}

void Arms::regDescriptorUpdate(Var * var) {
    std::map<Register, Var *>::iterator it = regDescriptorFind(var);
    it->second = var;
}

void Arms::discardVarInReg(Var * var, Register reg) {
    if (regs[reg].canDiscard) {
        regDescriptorRemove(var, reg);
        regs[reg].isDirty = false;
        regs[reg].canDiscard = false;
    }
}

void Arms::fillReg(Var * src, Register reg) {

}

void Arms::spillReg(Var * dst, Register reg) {

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