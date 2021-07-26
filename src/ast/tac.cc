#include <ast/tac.h>
#include <string>
#include <cassert>
namespace kisyshot::ast {

    //检查变量是否全局变量,给iswhat赋值
    Var::Var(std::string variableName) : variableName(variableName) {
        //TODO: 根据变量名判断是否是全局变量
        isWhat  = 2;
    }

//常量赋值
    Var::Var(int value) : value(value), isWhat(3) {}


    bool Var::isGlobal() { return isWhat == 1; }

    bool Var::isTemp() { return isWhat == 2; }

    bool Var::isConst() { return isWhat == 3; }

    bool Var::isLocal() { return isWhat == 4; }

    std::string Var::getName() {
        if (isConst())
            return std::to_string(value);
        else return variableName;
    }

    int Var::getOffset() {
        assert(isGlobal() || isLocal());
        return 0;
    }

    int Var::getBase() {
        assert(isGlobal() || isLocal());
        return 0;
    }


    std::string Binary_op::opName[Binary_op::NumOps] = {"+", "-", "*", "/", "%", "<", ">", "==", "!=", ">=" "<="};

    //TODO: 把 = 号从二元式中分离出来
    Binary_op::Binary_op(OpCode c, Var *src_1, Var *src_2, Var *Dst) :
            code(c), src_1(src_1), src_2(src_2), dst(Dst) {
        assert( src_1 != nullptr && src_2 != nullptr);
        assert(code >= 0 && code < NumOps);
        assert(!dst->isConst());
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
        //assert(label != nullptr);
    }

    std::string GOTO::toString() { return "GOTO " + label; }

    InstructionType GOTO::getType() {
        return InstructionType::GOTO_;
    }

    Label::Label(std::string &label) : label(label) {
        //assert(label != nullptr);
    }

    std::string Label::toString() { return label + ":"; }

    InstructionType Label::getType() {
        return InstructionType::Label_;
    }

    IfZ::IfZ(Var *condition, std::string &trueLabel) : condition(condition), trueLabel(trueLabel) {
        assert(condition != nullptr );
    }

    std::string IfZ::toString() { return "IfZ " + condition->getName() + " GOTO " + trueLabel; }

    InstructionType IfZ::getType() {
        return InstructionType::IfZ_;
    }

    Assign::Assign(Var *t1, Var *t2) : t1(t1), t2(t2) {
        assert(t1 != nullptr && t2 != nullptr);
        assert(!t1->isGlobal());  //愿操作数不是全局变量
        assert(!t2->isGlobal());   //目的操作数是临时变量
    }

    std::string Assign::toString() { return t2->getName() + " = " + t1->getName(); }

    InstructionType Assign::getType() {
        return InstructionType::Assign_;
    }

    Load::Load(Var *src, Var *dst) : src(src), dst(dst) {
        assert(src != nullptr && dst != nullptr);
        assert(src->isGlobal());
        assert(dst->isTemp());
    }

    std::string Load::toString() {return dst->getName() + " = " + src->getName();}

    InstructionType Load::getType() {
        return InstructionType::Load_;
    }

    Store::Store(Var *src, Var *dst):src(src),dst(dst) {
        assert(src != nullptr && dst != nullptr);
        assert(dst->isGlobal());
        assert(src->isTemp() || src->isConst());
    }

    std::string Store::toString() {return dst->getName() + " = " + src->getName();}

    InstructionType Store::getType() {
        return InstructionType::Store_;
    }

    Param::Param(Var *par) :par(par){
        assert(par != nullptr);
    }

    std::string Param::toString() {return "parameter " + par->getName();}

    InstructionType Param::getType() {
        return InstructionType::Param_;
    }

    Call::Call(std::string &funLabel, int n):funLabel(funLabel),n(n) {
        //assert(funLabel != nullptr);
        assert(n > 0);
    }


    Call::Call(std::string &funLabel, int n, Var *result):funLabel(funLabel),n(n),result(result) {
        assert(n > 0);
    }
    std::string Call::toString() {
        if(result != nullptr)
            return result->getName() + " = call "+ funLabel + ",  " + std::to_string(n);
        else
            return "call "+ funLabel + ",  " + std::to_string(n);
    }

    InstructionType Call::getType() {
        return InstructionType::Call_;
    }

    Return::Return(Var *v) :v(v){
        assert(v != nullptr);
    }

    std::string Return::toString() {return "Return "+ v->getName();}

    InstructionType Return::getType() {
        return InstructionType::Return_;
    }

    BeginFunc::BeginFunc() {}

    std::string BeginFunc::toString() {return "BeginFunc";}

    InstructionType BeginFunc::getType() {
        return InstructionType::BeginFunc_;
    }

    void BeginFunc::setFrameSize(int numBytesForLocalsAndTemps) {}

    EndFunc::EndFunc() {}

    std::string EndFunc::toString() {return "EndFunc";}

    InstructionType EndFunc::getType() {
        return InstructionType::EndFunc_;
    }

}
