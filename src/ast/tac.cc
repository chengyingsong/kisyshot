#include <ast/tac.h>
#include <string>
#include <cassert>
namespace kisyshot::ast {

    //检查变量是否全局变量,给iswhat赋值
    Var::Var(std::string variableName) : variableName(variableName) {
        //TODO: 根据变量名判断是否是全局变量
        iswhat  = 2;
    }

//常量赋值
    Var::Var(int value) : value(value), iswhat(3) {}


    bool Var::isGlobal() { return iswhat == 1; }

    bool Var::isTemp() { return iswhat == 2; }

    bool Var::isConst() { return iswhat == 3; }


    std::string Var::getName() {
        if (isConst())
            return std::to_string(value);
        else return variableName;
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

    GOTO::GOTO(std::string &label) : label(label) {
        //assert(label != nullptr);
    }

    std::string GOTO::toString() { return "GOTO " + label; }

    Label::Label(std::string &label) : label(label) {
        //assert(label != nullptr);
    }

    std::string Label::toString() { return label + ":"; }

    IfZ::IfZ(Var *condition, std::string &trueLabel) : condition(condition), trueLabel(trueLabel) {
        assert(condition != nullptr );
    }

    std::string IfZ::toString() { return "IfZ " + condition->getName() + " GOTO " + trueLabel; }

    Assign::Assign(Var *t1, Var *t2) : t1(t1), t2(t2) {
        assert(t1 != nullptr && t2 != nullptr);
        assert(!t1->isGlobal());  //愿操作数不是全局变量
        assert(!t2->isGlobal());   //目的操作数是临时变量
    }

    std::string Assign::toString() { return t2->getName() + " = " + t1->getName(); }

    Load::Load(Var *src, Var *dst) : src(src), dst(dst) {
        assert(src != nullptr && dst != nullptr);
        assert(src->isGlobal());
        assert(dst->isTemp());
    }

    std::string Load::toString() {return dst->getName() + " = " + src->getName();}

    Store::Store(Var *src, Var *dst):src(src),dst(dst) {
        assert(src != nullptr && dst != nullptr);
        assert(dst->isGlobal());
        assert(src->isTemp() || src->isConst());
    }

    std::string Store::toString() {return dst->getName() + " = " + src->getName();}

    Param::Param(Var *par) :par(par){
        assert(par != nullptr);
    }

    std::string Param::toString() {return "parameter " + par->getName();}

    Call::Call(std::string &funLabel, int n):funLabel(funLabel),n(n) {
        //assert(funLabel != nullptr);
        assert(n > 0);
    }


    Call::Call(std::string &funLabel, int n, Var *result):funLabel(funLabel),n(n),result(result) {
        assert(n > 0);
    }
    std::string Call::toString() {
        return "call "+ funLabel + ",  " + std::to_string(n);
    }

    Return::Return(Var *v) :v(v){
        assert(v != nullptr);
    }

    std::string Return::toString() {return "Return "+ v->getName();}

    BeginFunc::BeginFunc() {}

    std::string BeginFunc::toString() {return "BeginFunc";}

    EndFunc::EndFunc() {}

    std::string EndFunc::toString() {return "EndFunc";}

}
