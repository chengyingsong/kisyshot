#pragma GCC diagnostic ignored "-Wpedantic"
#include <ast/arms.h>
#include <stdarg.h>
#include <stdio.h>
#include <string>

using namespace kisyshot::ast;

bool Arms::varsAreSame(Var * var1, Var * var2) {
    return (var1 == var2 || (var1 && var2 && (var1->getName() == var2->getName()) && (var1->getBase() == var2->getBase()) && getOffset(var1) == getOffset(var2)));
}

int Arms::getOffset(Var * var) {
    return ctx->symbols[var->getName()]->offset;
}

int Arms::findRegForVar(Var * var) {
    std::map<Register, Var *>::iterator it;
    Register reg = (Register)r0;
    bool flag = false;
    for (it = regDescriptor.begin(); it != regDescriptor.end(); it++) {
        if (varsAreSame(var, it->second)) {
            reg = it->first;
            flag = true;
        }
    }

    if (flag)
        return reg;
    else
        return -1;
}

int Arms::findCleanReg() {
    int index = -1;
    for (int i = r0; i <= r11; i++) {
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
    for (int i = r0; i <= r11; i++)
        if (regs[i].isDirty == true && i != r7)
            cleanReg((Register)i);
}

void Arms::cleanRegForEndFunc() {
    for (int i = r0; i <= r11; i++)
        if (i != r7)
            regs[i].isDirty = false;
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
    regDescriptor.erase(reg);
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
        regDescriptorRemove(var, reg);
        regs[reg].isDirty = false;
        regs[reg].canDiscard = false;
}

void Arms::fillReg(Var * src, Register reg) {
    Register preReg = (Register)findRegForVar(src);
    if (src->type == VarType::GlobalVar) {
        if ((int)preReg == -1)
            if (src->isArray)
                printf("\tldr %s, =%s\n", regs[reg].name.c_str(), src->getName().c_str());  
            else
                printf("\tldr %s, %s\n", regs[reg].name.c_str(), src->getName().c_str());
        else if (reg != preReg)
            printf("\tmov %s, %s\n", regs[reg].name.c_str(), regs[preReg].name.c_str());
    }
    if (src->type == VarType::LocalVar) {
        if ((int)preReg == -1)
            printf("\tldr %s, [r7, #%d]\n", regs[reg].name.c_str(), getOffset(src));
        else if (reg != preReg)
            printf("\tmov %s, %s\n", regs[reg].name.c_str(), regs[preReg].name.c_str());
    }
    if (src->type == VarType::ConstVar)
        printf("\tmov %s, #%s\n", regs[reg].name.c_str(), src->getName().c_str());
}

void Arms::spillReg(Var * dst, Register reg) {
    if (!(dst->isArray)) {
        if (dst->type == VarType::GlobalVar)
            printf("\tstr %s, %s\tspill %s into memory\n", regs[reg].name.c_str(), dst->getName().c_str(), dst->getName().c_str());
        if (dst->type == VarType::LocalVar)
            printf("\tstr %s, [r7, #%d]\tspill %s into memory\n", regs[reg].name.c_str(), getOffset(dst), dst->getName().c_str());
    }
}

Arms::Arms(const std::shared_ptr<Context> &context) {
    regs[r0] = (RegContents){NULL, (std::string)"r0", false, false, false};
    regs[r1] = (RegContents){NULL, (std::string)"r1", false, false, false};
    regs[r2] = (RegContents){NULL, (std::string)"r2", false, false, false};
    regs[r3] = (RegContents){NULL, (std::string)"r3", false, false, false};
    regs[r4] = (RegContents){NULL, (std::string)"r4", false, false, false};
    regs[r5] = (RegContents){NULL, (std::string)"r5", false, false, false};
    regs[r6] = (RegContents){NULL, (std::string)"r6", false, false, false};
    regs[r7] = (RegContents){NULL, (std::string)"r7", false, false, false};
    regs[r8] = (RegContents){NULL, (std::string)"r8", false, false, false};
    regs[r9] = (RegContents){NULL, (std::string)"r9", false, false, false};
    regs[r10] = (RegContents){NULL, (std::string)"r10", false, false, false};
    regs[r11] = (RegContents){NULL, (std::string)"r11", false, false, false};
    regs[r12] = (RegContents){NULL, (std::string)"r12", false, false, false};
    regs[sp] = (RegContents){NULL, (std::string)"sp", false, false, false};
    regs[lr] = (RegContents){NULL, (std::string)"lr", false, false, false};
    regs[pc] = (RegContents){NULL, (std::string)"pc", false, false, false};

    ctx = context;

    opName[0] = "add";
    opName[1] = "sub";
    opName[2] = "mul";
    opName[3] = "div";
    opName[4] = "no";
    opName[5] = "no";
    opName[6] = "no";
    opName[7] = "no";
    opName[8] = "no";
    opName[9] = "no";
    opName[10] = "no";   
}

void Arms::generateDiscardVar(Var * var) {
    rd = (Register)pickRegForVar(var);
    printf("\t# Last use of %s. Discard register %s\n", var->getName().c_str(), regs[rd].name.c_str());
    regs[rd].canDiscard = true;
}

void Arms::generateAssignConst(Var * dst, Var * src) {
    rd = (Register)pickRegForVar(dst);
    regs[rd].mutexLock = true;
    printf("\tmov %s, #%s", regs[rd].name.c_str(), src->getName().c_str());
    printf("\t# %s = %s\n", dst->getName().c_str(), src->getName().c_str());
    regDescriptorInsert(dst, rd);
    regs[rd].mutexLock = false;
    if (regs[rd].canDiscard)
        discardVarInReg(dst, rd);
}

void Arms::generateAssign(Var * dst, Var * src) {
    if (src->type == VarType::ConstVar) {
        generateAssignConst(dst, src);
        return;
    }
    if (src->type == VarType::GlobalVar) {
        rd = (Register)pickRegForVar(dst);
        regs[rd].mutexLock = true;
        printf("\tldr %s, %s", regs[rd].name.c_str(), src->getName().c_str());
        regDescriptorInsert(dst, rd);
        regs[rd].mutexLock = false;
        if (regs[rd].canDiscard)
            discardVarInReg(dst, rd);
        return;
    }
    rs = (Register)pickRegForVar(src);
    regs[rs].mutexLock = true;
    fillReg(src, rs);
    regDescriptorInsert(src, rs);
    rd = (Register)pickRegForVar(dst);
    regs[rd].mutexLock = true;
    printf("\tmov %s, %s", regs[rd].name.c_str(), regs[rs].name.c_str());
    regDescriptorInsert(dst, rd);
    regs[rs].mutexLock = false;
    regs[rd].mutexLock = false;
    if (regs[rs].canDiscard)
        discardVarInReg(src, rs);
    if (regs[rd].canDiscard)
        discardVarInReg(dst, rd);
    printf("\t# %s = %s\n", dst->getName().c_str(), src->getName().c_str());
}

void Arms::generateLoad(Var * dst, Var * src, Var * offset) {
    rt = (Register)pickRegForVar(offset);
    regs[rt].mutexLock = true;
    fillReg(offset, rt);
    regDescriptorInsert(offset, rt);
    if (src->type == VarType::GlobalVar) {
        rd = (Register)pickRegForVar(dst);
        regs[rd].mutexLock = true;
        printf("\tldr %s, [%s, %s, lsl #2]", regs[rd].name.c_str(), src->getName().c_str(), regs[rt].name.c_str());
        regDescriptorInsert(dst, rd);
        regs[rd].mutexLock = false;
        if (regs[rd].canDiscard)
            discardVarInReg(dst, rd);
        return;
    }
    rs = (Register)pickRegForVar(src);
    regs[rs].mutexLock = true;
    if (getRegContents(rs) != src)
        printf("\tadd %s, [r7], #%d\n", regs[rs].name.c_str(), getOffset(src));
    regDescriptorInsert(src, rs);
    rd = (Register)pickRegForVar(dst);
    regs[rd].mutexLock = true;
    regDescriptorInsert(dst, rd);
    printf("\tldr %s, [%s, %s, lsl #2]", regs[rd].name.c_str(), regs[rs].name.c_str(), regs[rt].name.c_str());
    regs[rs].mutexLock = false;
    regs[rd].mutexLock = false;
    if (regs[rs].canDiscard)
        discardVarInReg(src, rs);
    if (regs[rd].canDiscard)
        discardVarInReg(dst, rd);
    if (regs[rt].canDiscard)
        discardVarInReg(offset, rt);
    printf("\t# %s = %s[%s]\n", dst->getName().c_str(), src->getName().c_str(), offset->getName().c_str());
}

void Arms::generateStore(Var * dst, Var * offset, Var * src) {
    rt = (Register)pickRegForVar(offset);
    regs[rt].mutexLock = true;
    fillReg(offset, rt);
    regDescriptorInsert(offset, rt);
    if (dst->type == VarType::GlobalVar) {
        rs = (Register)pickRegForVar(src);
        regs[rs].mutexLock = true;
        fillReg(src, rs);
        regDescriptorInsert(src, rs);
        printf("\tstr %s, [%s, %s, lsl #2]", regs[rs].name.c_str(), dst->getName().c_str(), regs[rt].name.c_str());
        regs[rs].mutexLock = false;
        if (regs[rs].canDiscard)
            discardVarInReg(src, rs);
        return;
    }
    rs = (Register)pickRegForVar(src);
    regs[rs].mutexLock = true;
    fillReg(src, rs);
    regDescriptorInsert(src, rs);
    rd = (Register)pickRegForVar(dst);
    regs[rd].mutexLock = true;
    if (getRegContents(rd) != dst)
        printf("\tadd %s, [r7], #%d\n", regs[rd].name.c_str(), getOffset(dst));
    regDescriptorInsert(dst, rd);
    printf("\tstr %s, [%s, %s, lsl #2]", regs[rs].name.c_str(), regs[rd].name.c_str(), regs[rt].name.c_str());
    regs[rs].mutexLock = false;
    regs[rd].mutexLock = false;
    if (regs[rs].canDiscard)
        discardVarInReg(src, rs);
    if (regs[rd].canDiscard)
        discardVarInReg(dst, rd);
    if (regs[rt].canDiscard)
        discardVarInReg(offset, rt);
    printf("\t# %s[%s] = %s\n", dst->getName().c_str(), offset->getName().c_str(), src->getName().c_str());
}

void Arms::generateBinaryOP(Binary_op::OpCode op, Var * dst, Var * src_1, Var * src_2) {
    rs = (Register)pickRegForVar(src_1);
    regs[rs].mutexLock = true;
    fillReg(src_1, rs);
    regDescriptorInsert(src_1, rs);
    rt = (Register)pickRegForVar(src_2);
    regs[rt].mutexLock = true;
    fillReg(src_2, rt);
    regDescriptorInsert(src_2, rt);
    rd = (Register)pickRegForVar(dst);
    regs[rd].mutexLock = true;
    regDescriptorInsert(dst, rd);
    printf("\t%s %s, %s, %s", opName[op].c_str(), regs[rd].name.c_str(), regs[rs].name.c_str(), regs[rt].name.c_str());
    regs[rs].mutexLock = false;
    regs[rt].mutexLock = false;
    regs[rd].mutexLock = false;
    if (regs[rs].canDiscard)
        discardVarInReg(src_1, rs);
    if (regs[rt].canDiscard)
        discardVarInReg(src_2, rt);  
    if (regs[rd].canDiscard)
        discardVarInReg(dst, rd);   
    printf("\t# %s = %s %s %s\n", dst->getName().c_str(), src_1->getName().c_str(), opName[op].c_str(), src_2->getName().c_str());
}

void Arms::generateLabel(std::string label) {
    if (label[0] != '.') {
        printf("\t.text\n");
        printf("\t.align 1\n");
        printf("\t.global %s\n", label.c_str());
        printf("\t.syntax unified\n");
        printf("\t.thumb\n");
        printf("\t.thumb_func\n");
        printf("\t.fpu neon\n");
        printf("\t.type %s, %%function\n", label.c_str());
    }
    printf("%s:\n", label.c_str());
}

void Arms::generateGOTO(std::string label) {
//    cleanRegForBranch();
    printf("\tb %s\n", label.c_str());
}

void Arms::generateIfZ(Var * test, std::string label) {
    fillReg(test, r12);
//    cleanRegForBranch();
    printf("\tcmp %s\n", regs[r12].name.c_str());
    printf("\tbeq %s\n", label.c_str());
    printf("\t# beq %s, %s\n", test->getName().c_str(), label.c_str());
}

void Arms::generateBeginFunc(std::string curFunc, int frameSize) {
    printf("\tpush {r7, lr}\n");
    printf("\tsub sp, sp, #%d\n", frameSize);
    int parNum = ctx->functions[curFunc]->params.size();
    for (int i = 0; i < parNum; i++)
        printf("\tstr r%d, [r7, #%d]\n", i, i * 4);
}

void Arms::generateEndFunc(std::string curFunc, int frameSize) {
    printf("\tadd sp, sp, #%d\n", frameSize);
    printf("\tmov sp, r7\n");
    printf("\tpop {r7, lr}\n");
    printf("\tbx lr\n");
    printf("\t.size %s, .-%s\n", curFunc.c_str(), curFunc.c_str());
    cleanRegForEndFunc();
}

void Arms::generateReturn(Var * result) {
    rs = (Register)pickRegForVar(result);
    fillReg(result, rs);
    printf("\tmov r0, %s\n", regs[rs].name.c_str());
    printf("\t# return %s\n", result->getName().c_str());
}

void Arms::generateParam(Var * arg, int num) {
    if (num == 1)
        cleanRegForBranch();
    fillReg(arg, (Register)(num - 1));
    printf("\tparam %s\n", arg->getName().c_str());
}

void Arms::generateCall(int numVars, std::string label, Var * result, int paramNum) {
    if (paramNum == 0)
        cleanRegForBranch();
    if (numVars == 1) {
        printf("\tbl %s\n", label.c_str());
        rd = (Register)pickRegForVar(result);
        fillReg(result, rd);
        regDescriptorInsert(result, rd);
        printf("\tmov %s, r0", regs[rd].name.c_str());
        printf("\t# %s = %s\n", result->getName().c_str(), label.c_str());
    }
    else
        printf("\tbl %s\n", label.c_str());
}

void Arms::generateHeaders() {
    printf("\t.arch armv7-a\n");
	printf("\t.eabi_attribute 28, 1\n");
	printf("\t.eabi_attribute 20, 1\n");
	printf("\t.eabi_attribute 21, 1\n");
	printf("\t.eabi_attribute 23, 3\n");
	printf("\t.eabi_attribute 24, 1\n");
	printf("\t.eabi_attribute 25, 1\n");
	printf("\t.eabi_attribute 26, 2\n");
	printf("\t.eabi_attribute 30, 6\n");
	printf("\t.eabi_attribute 34, 1\n");
	printf("\t.eabi_attribute 18, 4\n");
}

void Arms::generateGlobal() {
    printf("There are globals\n");
}