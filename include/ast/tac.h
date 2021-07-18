#pragma once
#define Numops 11

#include "variable.h"

namespace kisyshot::ast {
    class Var {
        /*
        Var是表示四元式里，操作数的数据结构。操作数有临时变量类型，符号表指针类型和整型
        */
    public:
        TempVar temp;
        SymbolTable * p;
        int which = 0;  //Which为0表示空，1为临时变量，2为符号表条目
        Var(TempVar temp);
        Var(SymbolTable * p);
    };

    class Instruction {
        
        public:
            virtual void print();
            virtual void toString();

            void generate();

            Var  src_1;
            Var  src_2;
            Var  dst;
            
            int numVars;
    };

    class Binary_op : Instruction{
        public:
        typedef enum {Add,Sub,Mul,Div,Mod,Less,Greater,Equaleq,Exclaimeq,Greatereq,Lesseq} OpCode; //后面是关系表达式
        static const char* const opName[Numops]; //每一个索引对应的运算符号
        static OpCode opCodeForName(const char* name);

        OpCode code;
        Var src_1,src_2,dst;
        

        Binary_op(OpCode c,Var &src_1,Var &src_2,Var &Dst);
        void generate();
    };
    //中间代码生成后无 逻辑表达式，也就是没有A && B

    class GOTO :Instruction {
    public:
        int label;
        GOTO(int label);
        void genrate();
    };

    class If :Instruction {
    public:
        Var condition;
        int trueLabel;
        int falseLabel;
        If(Var& condition,int trueLabel,int falseLabel);  
    };

    class Assign : Instruction {
    public:
        TempVar  t1;
        TempVar  t2;  
        //t2 = t1
        Assign(TempVar&t1,TempVar&t2);
    };


    class Load : Instruction {
    public:
        TempVar t1;
        SymbolTable * p;
        //从p中取数赋值给t1
        Load(TempVar &t1,SymbolTable *p);
    };


    class Store : Instruction {
    public:
        TempVar &t1;
        SymbolTable * p;
        //从t1赋值给p
        Store(TempVar &t1,SymbolTable *p);
    };

    class Param : Instruction {
    public:
        Var par;
        Param(Var &par);
    };

    class Call : Instruction {
    public:
        int funLabel;
        int n; //参数个数
        Call(int funLabel,int n);
    };

    class Return: Instruction {
        Var v;
    public:
        Return(Var & v);

    };

    class BeginFunc : Instruction {
        int frameSize;
    public:
        BeginFunc();
        void setFrameSize(int numBytesForLocalsAndTemps); 
        
    };

    class EndFunc : Instruction {
    public:
        EndFunc();

    };

}
