#include<compiler/codegen.h>
#include<iostream>

using namespace kisyshot::ast;
using namespace kisyshot::compiler;


namespace kisyshot::compiler {


    CodeGenerator::CodeGenerator() {}


/*    CodeGenerator::CodeGenerator(const std::shared_ptr<Context> &context){
        Symbol = context->symbols;
        Function = context->functions;
    }*/

    //返回一个不重复的标号
    std::string CodeGenerator::newLabel() {
        static int labelId = 0;
        labelId++;
        return ".L" + std::to_string(labelId);
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
    void CodeGenerator::genLoad(Var *src_1,Var* src_2,Var *dst) {
        Load *p = new Load(src_1, src_2,dst);
        code.push_back((Instruction *)p);
        printInstruction((Instruction *)p);
    }

    void CodeGenerator::genAssign(Var *src, Var *dst) {
        auto p = new Assign(src, dst);
        code.push_back((Instruction *)p);
        printInstruction((Instruction *)p);
    }

    void CodeGenerator::genStore(Var *src_1, Var* src_2,Var *dst) {
        auto p = new Store(src_1,src_2,dst);
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
        if(const2VarMap.find(value) == const2VarMap.end()){
            Var * t = new Var(value);
            const2VarMap[value] = t;
        }
        return const2VarMap[value];
    }


    void CodeGenerator::genCMP(ast::TokenType opType,ast::Var* src_1,ast::Var* src_2, std::string &label){
        auto p = new CMP(opType,src_1,src_2,label);
        code.push_back((Instruction *)p);
        printInstruction((Instruction *)p);
    }


}