#include <ast/tac.h>
#include <string>
#include <cassert>

namespace kisyshot::ast {

    //检查变量是否全局变量,给iswhat赋值
    Var::Var(std::string variableName) : variableName(variableName) {
        //TODO: 根据传入的重整变量名判断全局变量，临时变量还是局部变量
        //如果重整名没有@就是全局变量,如果前面是_temp_就是临时变量
        if (variableName.find("@") != variableName.npos)
            type = VarType::LocalVar;
        else if (variableName.find("_temp_") != variableName.npos)
            type = VarType::TempVar;
        else type = VarType::GlobalVar;
    }

    //常量赋值
    Var::Var(int value) : value(value) {
        type = VarType::ConstVar;
    }

    Var::Var() {}

    std::string Var::getName() {
        if (type == VarType::ConstVar)
            return std::to_string(value);
        else
            return variableName;
    }

    std::string Var::getBase() {
        assert(type = VarType::LocalVar);
        size_t index = -1;
        while (variableName.find("@", index + 1) != variableName.npos) {
            index = variableName.find("@", index + 1);
        }
        return variableName.substr(index + 1, variableName.npos - index);
    }


    std::string Binary_op::opName[Binary_op::NumOps] = {"+", "-", "*", "/", "%", "<", ">", "==", "!=", ">=" "<="};

    //TODO: 把 = 号从二元式中分离出来
    Binary_op::Binary_op(OpCode c, Var *src_1, Var *src_2, Var *Dst) :
            Instruction(src_1, src_2, Dst), code(c) {
        numVars = 3;
        assert(src_1 != nullptr && src_2 != nullptr);
        assert(code >= 0 && code < NumOps);
        assert(Dst->type != VarType::ConstVar || Dst->type != VarType::ConstVar);
    }

    std::string Binary_op::toString() {
        return dst->getName() + " = " + src_1->getName() + " " + opName[code] + " " + src_2->getName();
    }

    Binary_op::OpCode Binary_op::opCodeForName(std::string &name) {
        for (int i = 0; i < NumOps; i++) {
            if (opName[i] == name)
                return (OpCode) i;
        }
        return Add;
    }

    InstructionType Binary_op::getType() {
        return InstructionType::Binary_op_;
    }

    GOTO::GOTO(std::string &label) : label(label) {
        numVars = 0;
        //assert(label != nullptr);
    }

    std::string GOTO::toString() { return "GOTO " + label; }

    InstructionType GOTO::getType() {
        return InstructionType::GOTO_;
    }

    Label::Label(std::string &label) : label(label) {
        numVars = 0;
        //assert(label != nullptr);
    }

    std::string Label::toString() { return label + ":"; }

    InstructionType Label::getType() {
        return InstructionType::Label_;
    }

    IfZ::IfZ(Var *condition, std::string &trueLabel) : Instruction(condition), trueLabel(trueLabel) {
        numVars = 1;
        assert(condition != nullptr);
    }

    std::string IfZ::toString() { return "IfZ " + src_1->getName() + " GOTO " + trueLabel; }

    InstructionType IfZ::getType() {
        return InstructionType::IfZ_;
    }

    Assign::Assign(Var *t1, Var *t2) : Instruction(t1, t2) {
        numVars = 2;
        assert(t1 != nullptr && t2 != nullptr);
        //assert(t1->type != VarType::GlobalVar);  //愿操作数不是全局变量
//        assert(t1->type != VarType::GlobalVar);   //目的操作数是临时变量
    }

    std::string Assign::toString() { return src_2->getName() + " = " + src_1->getName(); }

    InstructionType Assign::getType() {
        return InstructionType::Assign_;
    }

    Load::Load(Var *src_1, Var *src_2, Var *dst) : Instruction(src_1, src_2, dst) {
        numVars = 3;
        assert(src_1 != nullptr && src_2 != nullptr && dst != nullptr);
        assert(src_1->isArray);  //第一个是数组Base

    }

    std::string Load::toString() { return dst->getName() + " = " + src_1->getName() + "[" + src_2->getName() + "]"; }

    InstructionType Load::getType() {
        return InstructionType::Load_;
    }

    Store::Store(Var *src_1, Var *src_2, Var *dst) : Instruction(src_1, src_2, dst) {
        numVars = 3;
        assert(src_1 != nullptr && src_2 != nullptr && dst != nullptr);
        assert(src_2->isArray);
    }

    std::string Store::toString() { return src_2->getName() + "[" + dst->getName() + "]" + " = " + src_1->getName(); }

    InstructionType Store::getType() {
        return InstructionType::Store_;
    }

    Param::Param(Var *par) : Instruction(par) {
        numVars = 1;
        assert(par != nullptr);
    }

    std::string Param::toString() { return "parameter " + src_1->getName(); }

    InstructionType Param::getType() {
        return InstructionType::Param_;
    }

    Call::Call(std::string &funLabel, int n) : funLabel(funLabel), n(n) {
        numVars = 0;
        //assert(funLabel != nullptr);
        assert(n >= 0);
    }


    Call::Call(std::string &funLabel, int n, Var *result) : Instruction(result), funLabel(funLabel), n(n) {
        numVars = 1;
        assert(n >= 0);
    }

    std::string Call::toString() {
        if (numVars == 1)
            return src_1->getName() + " = call " + funLabel + ",  " + std::to_string(n);
        else
            return "call " + funLabel + ",  " + std::to_string(n);
    }

    InstructionType Call::getType() {
        return InstructionType::Call_;
    }

    Return::Return(Var *v) : Instruction(v) {
        numVars = 1;
    }

    std::string Return::toString() {
        if (src_1 == nullptr) return "Return;";
        else return "Return " + src_1->getName();
    }

    InstructionType Return::getType() {
        return InstructionType::Return_;
    }

    BeginFunc::BeginFunc() { numVars = 0; }

    std::string BeginFunc::toString() { return "BeginFunc\nstackSize:  " + std::to_string(frameSize); }

    InstructionType BeginFunc::getType() {
        return InstructionType::BeginFunc_;
    }

    void BeginFunc::setFrameSize(int numBytesForLocalsAndTemps) {}

    EndFunc::EndFunc() { numVars = 0; }

    std::string EndFunc::toString() { return "EndFunc"; }

    InstructionType EndFunc::getType() {
        return InstructionType::EndFunc_;
    }

    Instruction::Instruction() {}

    Instruction::Instruction(Var *src_1, Var *src_2) : src_1(src_1), src_2(src_2) {}

    Instruction::Instruction(Var *src_1, Var *src_2, Var *dst) : src_1(src_1), src_2(src_2), dst(dst) {}

    Instruction::Instruction(Var *src_1) : src_1(src_1) {}


    CMP::CMP(TokenType opType, Var *src_1, Var *src_2, std::string &label) : Instruction(src_1, src_2), opType(opType),
                                                                            label(label) {
        numVars = 2;
    }

    InstructionType CMP::getType() {
        return InstructionType::Param_;
    }

    std::string CMP::toString() {
        return "if " + src_1->getName() +"  "+ getTokenSpell(opType) + "  " + src_2->getName() +"  GOTO "+label;
    }
}
