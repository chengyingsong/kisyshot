#pragma once


#include<list>
#include<ast/tac.h>
#include<string>
#include<unordered_map>

namespace kisyshot::compiler {
    //一个中间代码生成类
    class CodeGenerator {
        //TODO: 定义一个输出所有代码的方法
        //TODO: call语句和表达式语句在生成中间代码的同时应该返回一个临时变量来保存返回值
    public:
        std:: list < ast::Instruction * > code;
        //在变量声明时加入
        std::unordered_map<std::string,ast::Var* > name2VarMap;
        bool printOrNot = 1;

        CodeGenerator();

        void printInstruction(ast::Instruction * p);

        //返回一个不重复的标号的编号
        std::string newLabel();
        //返回一个不重复的临时变量的编号
        ast::Var* newTempVar();
        //绑定符号表变量和临时变量

        void genLabel(std::string &label);
        //Load指令
        void genLoad(ast::Var* src,ast::Var* dst);

        void genAssign(ast::Var* src,ast::Var* dst);

        void genStore(ast::Var* src,ast::Var* dst);

        void genBinaryOp(std::string &opName,ast::Var* op1,ast::Var* op2,ast::Var * dst);

        void genParam(ast::Var* pa);

        void genCall(std::string &funLabelId,int n,ast::Var* dst);

        void genCallNoReturn(std::string &funLabelId,int n);

        void genIFZ(ast::Var* condition,std::string &label);

        void genGOTO(std::string &label);

        void genReturn(ast::Var* v);

        void genBeginFunc();

        void genEndFunc();
    };
}



