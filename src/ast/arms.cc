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
    for (int i = r0; i <= r9; i++) {
        if ((regs[i].isDirty == false) && (regs[i].mutexLock == false)) {
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
        select = rand() % 10;
        if (regs[select].mutexLock == false) {
            if (getRegContents((Register)select) != NULL) {
                if (getRegContents((Register)select)->type == VarType::GlobalVar)
                    spillReg(getRegContents((Register)select), (Register)select);
                else if (getRegContents((Register)select)->type == VarType::LocalVar)
                    spillReg(getRegContents((Register)select), (Register)select);
                else if (getRegContents((Register)select)->type == VarType::TempVar) {
                    stack.push_back(getRegContents((Register)select));
                    fprintf(fp, "\tpush {%s}\n", regs[select].name.c_str());
                    spillReg(getRegContents((Register)select), (Register)select);
                }
            }
            flag = true;
        }
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
    if (var != NULL)
        spillReg(var, reg);
    regs[reg].isDirty = false;
}

void Arms::cleanRegForBranch() {
    for (int i = r0; i <= r9; i++)
        if (regs[i].isDirty)
            if (getRegContents((Register)i) != NULL) {
                if (getRegContents((Register)i)->type == VarType::GlobalVar)
                    cleanReg((Register)i);
                else if (getRegContents((Register)i)->type == VarType::LocalVar)
                    cleanReg((Register)i);
            }
}

void Arms::cleanRegForCall() {
    for (int i = r0; i <= r9; i++)
        if (regs[i].isDirty == true)
            if (getRegContents((Register)i) != NULL) {
                if (getRegContents((Register)i)->type != VarType::TempVar)
                    spillReg(getRegContents((Register)i), (Register)i);
                else {
                    VarStack.push_back(getRegContents((Register)i));
                    regStack.push_back(i);
                    regDescriptorRemove(getRegContents((Register)i), (Register)i);
            }
        }
    for (size_t i = 0; i < regStack.size(); i++) {
        if (i == 0)
            fprintf(fp, "\tpush {");
        if (i == regStack.size() - 1)
            fprintf(fp, "r%d}\n", regStack[i]);
        else
            fprintf(fp, "r%d, ", regStack[i]);
    }
}

void Arms::cleanRegForEndFunc() {
    for (int i = r0; i <= r9; i++)
        if (regs[i].isDirty) 
            if (getRegContents((Register)i) != NULL) {
                if (getRegContents((Register)i)->type == VarType::GlobalVar)
                    cleanReg((Register)i);
                else if (getRegContents((Register)i)->type == VarType::LocalVar)
                    cleanReg((Register)i);
            }   
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

void Arms::regDescriptorInsert(Var * var, Register reg, bool dirty = true) {
    if (regDescriptor.find((Register)reg) != regDescriptor.end())
        regDescriptor[reg] = var;
    else 
        regDescriptor.insert(std::pair<Register, Var *>(reg, var));
    regs[reg].isDirty = dirty;
}

void Arms::regDescriptorRemove(Var * var, Register reg) {
    if (regDescriptor.find(reg) != regDescriptor.end())
        regDescriptor.erase(reg);
    regs[reg].isDirty = false;
}

void Arms::discardVarInReg(Var * var, Register reg) {
        regDescriptorRemove(var, reg);
        regs[reg].isDirty = false;
        regs[reg].canDiscard = false;
}

void Arms::fillReg(Var * src, Register reg) {
    Register preReg = (Register)findRegForVar(src);
    if (src->type == VarType::StringVar) {
        fprintf(fp, "\tmov32I %s, %s\n", regs[reg].name.c_str(), src->getName().c_str());
    }
    if (src->type == VarType::GlobalVar) {
        if ((int)preReg == -1)
            if (src->isArray)
                fprintf(fp, "\tmov32I %s, %s\n", regs[reg].name.c_str(), src->getName().c_str());  
            else {
                fprintf(fp, "\tmov32I %s, %s\n", regs[reg].name.c_str(), src->getName().c_str());
                fprintf(fp, "\tldr %s, [%s]\n", regs[reg].name.c_str(), regs[reg].name.c_str());
            }
        else if (reg != preReg)
            fprintf(fp, "\tmov %s, %s\n", regs[reg].name.c_str(), regs[preReg].name.c_str());
    }
    if (src->type == VarType::LocalVar) {
        if ((int)preReg == -1)
            if (src->isArray) {
                if (src->isParam)
                    fprintf(fp, "\tldr %s, [fp, #-%d]\n", regs[reg].name.c_str(), curFuncFrameSize - getOffset(src));
                else {
                    if (curFuncFrameSize - getOffset(src) > 255) {
                        fprintf(fp, "\tmov32I r10, 0x%08x\n", getOffset(src) - curFuncFrameSize);
                        fprintf(fp, "\tadd %s, fp, r10\n", regs[reg].name.c_str());
                    }
                    else
                        fprintf(fp, "\tadd %s, fp, #-%d\n", regs[reg].name.c_str(), curFuncFrameSize - getOffset(src));
                }
            }
            else
                fprintf(fp, "\tldr %s, [fp, #-%d]\n", regs[reg].name.c_str(), curFuncFrameSize - getOffset(src));
        else if (reg != preReg)
            fprintf(fp, "\tmov %s, %s\n", regs[reg].name.c_str(), regs[preReg].name.c_str());
    }
    if (src->type == VarType::ConstVar) {
        if (src->value > 65535 || src->value < 0)
            fprintf(fp, "\tmov32I %s, 0x%08x\n", regs[rd].name.c_str(), src->value);
        else
            fprintf(fp, "\tmov %s, #%d\n", regs[reg].name.c_str(), src->value);
    }
    if (src->type == VarType::TempVar) {
        if (preReg == -1) {
            for (int index = (int)stack.size() - 1; index >= 0; index--) {
                if (varsAreSame(stack[index], src)) {
                    if (index == (int)stack.size() - 1) {
                        fprintf(fp, "\tpop {%s}\n", regs[reg].name.c_str());
                        stack.pop_back();
                    }
                    else {
                        fprintf(fp, "\tldr %s, [sp, #%u]\n", regs[reg].name.c_str(), (stack.size() - index - 1) * 4);
                    }
                    break;
                }
            } 
        }
        else if (reg != preReg)
            fprintf(fp, "\tmov %s, %s\n", regs[reg].name.c_str(), regs[preReg].name.c_str());
    }
}

void Arms::spillReg(Var * dst, Register reg) {
    if (!(dst->isArray)) {
        if (dst->type == VarType::GlobalVar) {
            fprintf(fp, "\tmov32I %s, %s\n", regs[r10].name.c_str(), dst->getName().c_str());
            fprintf(fp, "\tstr %s, [%s]\t@ spill %s into memory\n", regs[reg].name.c_str(), regs[r10].name.c_str(), dst->getName().c_str());
        }
        if (dst->type == VarType::LocalVar)
            fprintf(fp, "\tstr %s, [fp, #-%d]\t@ spill %s into memory\n", regs[reg].name.c_str(), curFuncFrameSize - getOffset(dst), dst->getName().c_str());
    }
    regDescriptorRemove(dst, reg);
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
    regs[Fp] = (RegContents){NULL, (std::string)"fp", false, false, false};
    regs[ip] = (RegContents){NULL, (std::string)"ip", false, false, false};
    regs[sp] = (RegContents){NULL, (std::string)"sp", false, false, false};
    regs[lr] = (RegContents){NULL, (std::string)"lr", false, false, false};
    regs[pc] = (RegContents){NULL, (std::string)"pc", false, false, false};

    ctx = context;

    opName[0] = "add";
    opName[1] = "sub";
    opName[2] = "mul";
    opName[3] = "sdiv";
    opName[4] = "no";
    opName[5] = "no";
    opName[6] = "no";
    opName[7] = "no";
    opName[8] = "no";
    opName[9] = "no";
    opName[10] = "no";

    fp = fopen(context->target.c_str(), "w+");
}

void Arms::generateDiscardVar(Var * var) {
    int reg = findRegForVar(var);
    if (reg != -1)
        discardVarInReg(var, (Register)reg);
}

void Arms::generateAssignConst(Var * dst, Var * src) {
    rd = (Register)pickRegForVar(dst);
    regs[rd].mutexLock = true;
    fillReg(dst, rd);
    regDescriptorInsert(dst, rd);
    if (src->value > 65535 || src->value < 0)
        fprintf(fp, "\tmov32I %s, 0x%08x\n", regs[rd].name.c_str(), src->value);
    else
        fprintf(fp, "\tmov %s, #%d", regs[rd].name.c_str(), src->value);
    fprintf(fp, "\t@ %s = %s\n", dst->getName().c_str(), src->getName().c_str());
    regs[rd].mutexLock = false;
    if (dst->type == VarType::LocalVar || dst->type == VarType::GlobalVar)
        spillReg(dst, rd);
}

void Arms::generateAssign(Var * dst, Var * src) {
    if (src->type == VarType::ConstVar) {
        generateAssignConst(dst, src);
        return;
    }
    rs = (Register)pickRegForVar(src);
    regs[rs].mutexLock = true;
    fillReg(src, rs);
    regDescriptorInsert(src, rs);

    rd = (Register)pickRegForVar(dst);
    regs[rd].mutexLock = true;
    fillReg(dst, rd);
    regDescriptorInsert(dst, rd);

    fprintf(fp, "\tmov %s, %s", regs[rd].name.c_str(), regs[rs].name.c_str());
    regs[rs].mutexLock = false;
    regs[rd].mutexLock = false;
    fprintf(fp, "\t@ %s = %s\n", dst->getName().c_str(), src->getName().c_str());
    if (dst->type == VarType::LocalVar || dst->type == VarType::GlobalVar)
        spillReg(dst, rd);
}

void Arms::generateLoad(Var * dst, Var * src, Var * offset) {
    rs = (Register)pickRegForVar(src);
    regs[rs].mutexLock = true;
    fillReg(src, rs);
    regDescriptorInsert(src, rs);

    rd = (Register)pickRegForVar(offset);
    regs[rd].mutexLock = true;
    fillReg(offset, rd);
    regDescriptorInsert(offset, rd);

    rt = (Register)pickRegForVar(dst);
    regs[rt].mutexLock = true;
    fillReg(dst, rt);
    regDescriptorInsert(dst, rt);

    fprintf(fp, "\tldr %s, [%s, %s, lsl #2]", regs[rt].name.c_str(), regs[rs].name.c_str(), regs[rd].name.c_str());
    regs[rs].mutexLock = false;
    regs[rd].mutexLock = false;
    regs[rt].mutexLock = false;
    if (offset->type == VarType::ConstVar)
        discardVarInReg(offset, rd);
    fprintf(fp, "\t@ %s = %s[%s]\n", dst->getName().c_str(), src->getName().c_str(), offset->getName().c_str());
    if (dst->type == VarType::LocalVar || dst->type == VarType::GlobalVar)
        spillReg(dst, rt);
}

void Arms::generateStore(Var * dst, Var * offset, Var * src) {
    rs = (Register)pickRegForVar(src);
    regs[rs].mutexLock = true;
    fillReg(src, rs);
    regDescriptorInsert(src, rs);

    rd = (Register)pickRegForVar(offset);
    regs[rd].mutexLock = true;
    fillReg(offset, rd);
    regDescriptorInsert(offset, rd);

    rt = (Register)pickRegForVar(dst);
    regs[rt].mutexLock = true;
    fillReg(dst, rt);
    regDescriptorInsert(dst, rt);

    fprintf(fp, "\tstr %s, [%s, %s, lsl #2]", regs[rs].name.c_str(), regs[rt].name.c_str(), regs[rd].name.c_str());
    regs[rs].mutexLock = false;
    regs[rd].mutexLock = false;
    regs[rt].mutexLock = false;
    if (offset->type == VarType::ConstVar)
        discardVarInReg(offset, rd);
    fprintf(fp, "\t@ %s[%s] = %s\n", dst->getName().c_str(), offset->getName().c_str(), src->getName().c_str());
}

void Arms::generateBinaryOP(Binary_op::OpCode op, Var * dst, Var * src_1, Var * src_2) {
    rs = (Register)pickRegForVar(src_1);
    regs[rs].mutexLock = true;
    fillReg(src_1, rs);
    regDescriptorInsert(src_1, rs);

    rd = (Register)pickRegForVar(src_2);
    regs[rd].mutexLock = true;
    fillReg(src_2, rd);
    regDescriptorInsert(src_2, rd);

    rt = (Register)pickRegForVar(dst);
    regs[rt].mutexLock = true;
    fillReg(dst, rt);
    regDescriptorInsert(dst, rt);

    fprintf(fp, "\t%s %s, %s, %s", opName[op].c_str(), regs[rt].name.c_str(), regs[rs].name.c_str(), regs[rd].name.c_str());
    regs[rs].mutexLock = false;
    regs[rt].mutexLock = false;
    regs[rd].mutexLock = false; 
    if (src_1->type == VarType::ConstVar)
        discardVarInReg(src_1, rs);
    if (src_2->type == VarType::ConstVar)
        discardVarInReg(src_2, rd);
    fprintf(fp, "\t@ %s = %s %s %s\n", dst->getName().c_str(), src_1->getName().c_str(), opName[op].c_str(), src_2->getName().c_str());
    if (dst->type == VarType::LocalVar || dst->type == VarType::GlobalVar)
        spillReg(dst, rt);
}

void Arms::generateLabel(std::string label) {
    if (label[0] != '.') {
        fprintf(fp, "\t.text\n");
        fprintf(fp, "\t.align 1\n");
        fprintf(fp, "\t.global %s\n", label.c_str());
        fprintf(fp, "\t.syntax unified\n");
        fprintf(fp, "\t.type %s, %%function\n", label.c_str());
        curFuncFrameSize = ctx->functions[curFuncLabel]->stackSize;
    }
    else
        cleanRegForBranch();
    fprintf(fp, "%s:\n", label.c_str());
}

void Arms::generateGOTO(std::string label) {
    cleanRegForBranch();
    fprintf(fp, "\tb %s\n", label.c_str());
}

void Arms::generateIfZ(Var * test, std::string label) {
   cleanRegForBranch();
    rs = (Register)pickRegForVar(test);
    regs[rs].mutexLock = true;
    fillReg(test, rs);
    regDescriptorInsert(test, rs);

    fprintf(fp, "\tcmp %s, #0\n", regs[rs].name.c_str());
    fprintf(fp, "\tbeq %s", label.c_str());
    fprintf(fp, "\t@ beq %s, %s\n", test->getName().c_str(), label.c_str());
    regs[rs].mutexLock = false;
    if (test->type == VarType::ConstVar)
        discardVarInReg(test, rs);
}

void Arms::generateCMP(TokenType opType, Var * src_1, Var * src_2, std::string label) {
    cleanRegForBranch();
    rs = (Register)pickRegForVar(src_1);
    regs[rs].mutexLock = true;
    fillReg(src_1, rs);
    regDescriptorInsert(src_1, rs);

    rd = (Register)pickRegForVar(src_2);
    regs[rd].mutexLock = true;
    fillReg(src_2, rd);
    regDescriptorInsert(src_2, rd);

    fprintf(fp, "\tcmp %s, %s\n", regs[rs].name.c_str(), regs[rd].name.c_str());
    if (opType == TokenType::op_equaleq) {
        fprintf(fp, "\tbeq %s", label.c_str());
        fprintf(fp, "\t@ %s == %s, goto %s\n", src_1->getName().c_str(), src_2->getName().c_str(), label.c_str());
    }
    if (opType == TokenType::op_exclaimeq) {
        fprintf(fp, "\tbne %s", label.c_str());
        fprintf(fp, "\t@ %s != %s, goto %s\n", src_1->getName().c_str(), src_2->getName().c_str(), label.c_str());
    }
    if (opType == TokenType::op_greater) {
        fprintf(fp, "\tbgt %s", label.c_str());
        fprintf(fp, "\t@ %s > %s, goto %s\n", src_1->getName().c_str(), src_2->getName().c_str(), label.c_str());
    }
    if (opType == TokenType::op_less) {
        fprintf(fp, "\tblt %s", label.c_str());
        fprintf(fp, "\t@ %s < %s, goto %s\n", src_1->getName().c_str(), src_2->getName().c_str(), label.c_str());
    }
    if (opType == TokenType::op_greatereq) {
        fprintf(fp, "\tbge %s", label.c_str());
        fprintf(fp, "\t@ %s >= %s, goto %s\n", src_1->getName().c_str(), src_2->getName().c_str(), label.c_str());
    }
    if (opType == TokenType::op_lesseq) {
        fprintf(fp, "\tble %s", label.c_str());
        fprintf(fp, "\t@ %s <= %s, goto %s\n", src_1->getName().c_str(), src_2->getName().c_str(), label.c_str());
    }
    regs[rs].mutexLock = false;
    regs[rd].mutexLock = false;
    if (src_1->type == VarType::ConstVar)
        discardVarInReg(src_1, rs);
    if (src_2->type == VarType::ConstVar)
        discardVarInReg(src_2, rd);
}

void Arms::generateBeginFunc(std::string curFunc, int frameSize) {
    fprintf(fp, "\tpush {fp, lr}\n");
    fprintf(fp, "\tadd fp, sp, #0\n");
    if (frameSize > 255) {
        fprintf(fp, "\tmov32I r10, 0x%08x\n", frameSize);
        fprintf(fp, "\tsub sp, sp, r10\n");
    }
    else
        fprintf(fp, "\tsub sp, sp, #%d\n", frameSize);
    int parNum = ctx->functions[curFunc]->params.size();
    if (parNum > 4)
        parNum = 4;
    for (int i = 0; i < parNum; i++)
        fprintf(fp, "\tstr r%d, [fp, #-%d]\n", i, frameSize - i * 4);
}

void Arms::generateEndFunc(std::string curFunc, int frameSize) {
    fprintf(fp, "\tadd sp, fp, #0\n");
    if (curFunc == "main")
        fprintf(fp, "\tpop {fp, pc}\n");
    else
        fprintf(fp, "\tpop {fp, lr}\n");
    fprintf(fp, "\tbx lr\n");
    fprintf(fp, "\t.size %s, .-%s\n", curFunc.c_str(), curFunc.c_str());
    regDescriptor.clear();
    stack.clear();
    for (int i = r0; i <= r9; i++)
        regs[i].isDirty = false;
}

void Arms::generateReturn(Var * result) {
    cleanRegForEndFunc();
    if (result != nullptr) {
        rs = (Register)pickRegForVar(result);
        regs[rs].mutexLock = true;
        fillReg(result, rs);
        fprintf(fp, "\tmov r0, %s\n", regs[rs].name.c_str());
        fprintf(fp, "\t@ return %s\n", result->getName().c_str());
        regs[rs].mutexLock = false;
    }
    return;
}

void Arms::generateParam(Var * arg, int num, int frame) {
    if (num == 1)
        cleanRegForCall();
    if (num <= 4) {
        if (arg->type == VarType::ConstVar || arg->type == VarType::GlobalVar || arg->type == VarType::LocalVar || arg->type == VarType::StringVar)
            fillReg(arg, (Register)(num - 1));
        else {
            size_t index = 0;
            for (index = 0; index < VarStack.size(); index++)
                if (varsAreSame(arg, VarStack[index]))
                    break;
            fprintf(fp, "\tldr r%d, [sp, #%u]\n", num - 1, index * 4);
        }
        fprintf(fp, "\t@ param %s\n", arg->getName().c_str());
    } else {
        if (arg->type == VarType::ConstVar || arg->type == VarType::GlobalVar || arg->type == VarType::LocalVar || arg->type == VarType::StringVar) {
            fillReg(arg, r4);   
            fprintf(fp, "\tstr r4, [sp, #-%d]\n", 8 + frame - (num - 1) * 4);
        }
        else {
            size_t index = 0;
            for (index = 0; index < VarStack.size(); index++)
                if (varsAreSame(arg, VarStack[index]))
                    break;
            fprintf(fp, "\tldr r4, [sp, #%u]\n", index * 4);
            fprintf(fp, "\tstr r4, [sp, #%d]\n", 8 + frame - (num - 1) * 4);
        }
        fprintf(fp, "\t@ param %s\n", arg->getName().c_str());
    }
}

void Arms::generateCall(int numVars, std::string label, Var * result, int paramNum) {
    if (paramNum == 0)
        cleanRegForCall();
    fprintf(fp, "\tbl %s\n", label.c_str());
    if (numVars == 1) {
        rd = (Register)pickRegForVar(result);
        regs[rd].mutexLock = true;
        fillReg(result, rd);
        regDescriptorInsert(result, rd);
        if (label == "__aeabi_idivmod")
            fprintf(fp, "\tmov %s, r1", regs[rd].name.c_str());
        else
            fprintf(fp, "\tmov %s, r0", regs[rd].name.c_str());
        fprintf(fp, "\t@ %s = %s\n", result->getName().c_str(), label.c_str());
        regs[rd].mutexLock = false;
    }
    for (size_t i = 0; i < VarStack.size(); i++) {
        int reg = pickRegForVar(VarStack[i]);
        regs[reg].mutexLock = true;
        fillReg(VarStack[i], (Register)reg);
        regDescriptorInsert(VarStack[i], (Register)reg);
        fprintf(fp, "\tpop {%s}\n", regs[reg].name.c_str());
        fprintf(fp, "\t@ %s\n", VarStack[i]->getName().c_str());
        regs[reg].mutexLock = false;
    }
    for (auto it = ParamDiscard.begin(); it != ParamDiscard.end(); it++)
        if (it->second)
            discardVarInReg(it->first, (Register)findRegForVar(it->first)); 
    VarStack.clear();
    regStack.clear();
}

void Arms::generateHeaders() {
    fprintf(fp, "\t.arch armv8-a\n");
    fprintf(fp, "\t.arch armv7ve\n");
    fprintf(fp, "\t.fpu vfp\n");
    fprintf(fp, "\t.macro mov32I, reg, val\n");
    fprintf(fp, "\t\tmovw \\reg, #:lower16:\\val\n");
    fprintf(fp, "\t\tmovt \\reg, #:upper16:\\val\n");
    fprintf(fp, "\t.endm\n");
}

void Arms::generateGlobal() {
    for (size_t i = 0; i < ctx->globals.size(); i++) {
        fprintf(fp, "\t.global %s\n", ctx->globals[i]->varName->toString().c_str());
        fprintf(fp, "\t.data\n");
        fprintf(fp, "\t.align 2\n");
        fprintf(fp, "\t.type %s, %%object\n", ctx->globals[i]->varName->toString().c_str());
        fprintf(fp, "\t.size %s, %u\n", ctx->globals[i]->varName->toString().c_str(), ctx->globals[i]->accumulation.front() * 4);
        fprintf(fp, "%s:\n", ctx->globals[i]->varName->toString().c_str());
        if (ctx->globals[i]->initialValue != nullptr) {
            for (size_t j = 0; j < ctx->globals[i]->values.size(); j++)
                fprintf(fp, "\t.word %d\n", ctx->globals[i]->values[j]);
        }
        else
            fprintf(fp, "\t.space %d\n", ctx->globals[i]->accumulation.front() * 4);
    }
    for(auto &s : ctx->strings) {
        fprintf(fp, "\t.rodata\n");
        fprintf(fp, "\t.align 2\n");
        fprintf(fp, "%s:\n", s.second.c_str());
        fprintf(fp, "\t.ascii \"%s\\000\"\n", s.first.c_str());
    }
    fprintf(fp, "\t.text\n");
    fprintf(fp, "\t.global __aeabi_idivmod\n");
}

void Arms::generateEnders() {
    fprintf(fp, "\t.section .note.GNU-stack,\"\",%%progbits\n");
}