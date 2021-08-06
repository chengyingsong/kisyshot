#pragma once


#include<list>
#include<ast/tac.h>
#include<string>
#include<unordered_map>
//#include "context.h"

namespace kisyshot::compiler {
    //一个中间代码生成类
    class CodeGenerator {
    public:
        std:: list < ast::Instruction * > code;
        //在变量声明时加入
        std::unordered_map<std::string,ast::Var* > name2VarMap;
        std::unordered_map<int,ast::Var*> const2VarMap;
        bool printOrNot = 1;

        CodeGenerator();

        ast::Var* getConstVar(int value);

        void printInstruction(ast::Instruction * p);

        //返回一个不重复的标号的编号
        std::string newLabel();
        //返回一个不重复的临时变量的编号
        ast::Var* newTempVar();
        //绑定符号表变量和临时变量


        void genLabel(std::string &label);
        //Load指令
        void genLoad(ast::Var* src_1,ast::Var* src_2,ast::Var* dst);

        void genAssign(ast::Var* src,ast::Var* dst);

        void genStore(ast::Var* src_1,ast::Var* src_2,ast::Var* dst);

        void genBinaryOp(std::string &opName,ast::Var* op1,ast::Var* op2,ast::Var * dst);

        void genParam(std::string funName,ast::Var* pa);

        void genCall(std::string &funLabelId,int n,ast::Var* dst);

        void genCallNoReturn(std::string &funLabelId,int n);

        void genIFZ(ast::Var* condition,std::string &label);

        void genCMP(ast::TokenType opType,ast::Var* src_1,ast::Var* src_2, std::string &label);

        void genGOTO(std::string &label);

        void genReturn(ast::Var* v);

        void genBeginFunc(int stackSize);

        void genEndFunc();


    };
}



