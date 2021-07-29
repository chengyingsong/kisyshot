#include<compiler/codegen.h>
#include<iostream>

using namespace kisyshot::ast;
using namespace kisyshot::compiler;


namespace kisyshot::compiler {

    CodeGenerator::CodeGenerator(){}

    //返回一个不重复的标号
    std::string CodeGenerator::newLabel() {
        static int labelId = 0;
        labelId++;
        return "_label_" + std::to_string(labelId);
    }


    //返回一个不重复的临时变量
    Var *CodeGenerator::newTempVar() {
        static int TempID = 0;
        TempID++;
        std::string name = "_temp_" + std::to_string(TempID);
        Var *t = new Var(name);
        name2VarMap[name] = t; //把声明的临时变量加入map中
        return t;
    }

    void CodeGenerator::genLabel(std::string &label) {
        Label *p = new Label(label);
        code.push_back((Instruction *)p);
        printInstruction((Instruction *)p);
    }


    //Load指令
    void CodeGenerator::genLoad(Var *src,Var *dst) {
        Load *p = new Load(src, dst);
        code.push_back((Instruction *)p);
        printInstruction((Instruction *)p);
    }

    void CodeGenerator::genAssign(Var *src, Var *dst) {
        auto p = new Assign(src, dst);
        code.push_back((Instruction *)p);
        printInstruction((Instruction *)p);
    }

    void CodeGenerator::genStore(Var *src, Var *dst) {
        auto p = new Store(src, dst);
        code.push_back((Instruction *)p);
        printInstruction((Instruction *)p);
    }

    void CodeGenerator::genBinaryOp(std::string &opName, Var *op1, Var *op2,Var* dst) {
        auto p = new Binary_op(Binary_op::opCodeForName(opName), op1, op2, dst);
        code.push_back((Instruction *)p);
        printInstruction((Instruction *)p);
    }

    void CodeGenerator::genParam(Var *pa) {
        auto p = new Param(pa);
        code.push_back((Instruction *)p);
        printInstruction((Instruction *)p);
    }

    void CodeGenerator::genCall(std::string &funLabel,int n,Var* dst) {
        auto p = new Call(funLabel, n,dst);
        code.push_back((Instruction *)p);
        printInstruction((Instruction *)p);
    }

    void CodeGenerator::genCallNoReturn(std::string &funLabelId, int n) {
        auto p = new Call(funLabelId,n);
        code.push_back((Instruction *)p);
        printInstruction((Instruction *)p);
    }

    void CodeGenerator::genIFZ(Var *condition, std::string &label) {
        auto p = new IfZ(condition, label);
        code.push_back((Instruction *)p);
        printInstruction((Instruction *)p);
    }

    void CodeGenerator::genGOTO(std::string &label) {
        auto p = new GOTO(label);
        code.push_back((Instruction *)p);
        printInstruction((Instruction *)p);
    }

    void CodeGenerator::genReturn(Var *v) {
        auto p = new Return(v);
        code.push_back((Instruction *)p);
        printInstruction((Instruction *)p);
    }

    void CodeGenerator::genBeginFunc(int stackSize) {
        auto p = new BeginFunc();
        p->setFrameSize(stackSize);
        code.push_back((Instruction *)p);
        printInstruction((Instruction *)p);
    }

    void CodeGenerator::genEndFunc() {
        auto p = new EndFunc();
        code.push_back((Instruction *)p);
        printInstruction((Instruction *)p);
    }

    void CodeGenerator::printInstruction(ast::Instruction *p) {
        if (printOrNot) {
            if(p->getType() == ast::InstructionType::Label_) {
                std::cout <<  p->toString() << std::endl;
            } else {
                if (p->getType() == ast::InstructionType::BeginFunc_ ||
                    p->getType() == ast::InstructionType::EndFunc_) {
                    std::cout << "  " << p->toString() << std::endl;
                } else {
                    std::cout << "    " << p->toString() << std::endl;
                }
            }
        }
    }

    ast::Var *CodeGenerator::getConstVar(int value) {
        if(const2Varmap.find(value) == const2Varmap.end()){
            Var * t = new Var(value);
            const2Varmap[value] = t;
        }
        return const2Varmap[value];
    }

}