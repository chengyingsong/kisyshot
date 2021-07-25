#pragma once

#include <string>
#include "variable.h"


namespace kisyshot::ast {
    class Var {
        /*
        Var是表示四元式里，操作数的数据结构。操作数有临时变量类型，全局变量类型和整型
        全局变量是基址和地址偏移，临时变量是变量名和变量值，局部变量就是变量名不一样而已。
         变量有重整名
        */
    public:
        std::string variableName;  //变量名，可以是变量名或者变量重整名
        int value;
        int iswhat;

        //传入一个变量名建立一个Var对象，需要判断是否是全局变量，是否是常数
        Var(std::string variableName);

        Var(int value);


        bool isGlobal();

        bool isConst();

        bool isTemp();

        std::string getName();
    };

    class Instruction {
        //一个基类，表示各种指令
        public:
            //返回指令的三地址码
            virtual std::string toString() = 0;
            //void generate();

            Var*  src_1;
            Var*  src_2;
            Var*  dst;
            //int numVars;
    };

    //统一把Dst放在最后。也就是左值一般是最后一个参数。

    class Binary_op : Instruction{
        public:
        static const int NumOps = 11;
        typedef enum {Add,Sub,Mul,Div,Mod,Less,Greater,Equaleq,Exclaimeq,Greatereq,Lesseq} OpCode; //后面是关系表达式
        static std::string opName[NumOps]; //每一个索引对应的运算符号
        //返回名字对应的code
        static OpCode opCodeForName(std::string &name);
        std::string toString() override;

        OpCode code;   //运算符号类型
        Var *src_1,*src_2,*dst;
        //一个构造器
        Binary_op(OpCode c,Var *src_1,Var *src_2,Var *Dst);
    };

    //中间代码生成后无 逻辑表达式，也就是没有A && B


    class GOTO :Instruction {
    public:
        std::string label;
        GOTO(std::string &label);
        std::string toString() override;
    };


    class Label :Instruction {
        std::string label;
    public:
        Label(std::string &label);
        std::string toString() override;
    };

    class IfZ :Instruction {
    public:
        //如果conditon的值为0则跳转到label处
        Var * condition;
        std::string trueLabel;
        IfZ(Var* condition,std::string &trueLabel);
        std::string toString() override;
    };

    class Assign : Instruction {
        //Assign是两个非全局变量之间的赋值
    public:
        Var*  t1;
        Var*  t2;
        Assign(Var* t1,Var *t2);
        std::string toString() override;
    };


    class Load : Instruction {
        //Load是将全局变量的值赋给一个局部变量，需要断言src是全局变量，dst不是全局变量
    public:
        Var* src;
        Var* dst;
        Load(Var *src,Var* dst);
        std::string toString() override;
    };


    class Store : Instruction {
        //Store是将局部变量的值赋给全局变量
    public:
        Var* src;
        Var* dst;
        Store(Var *src,Var *dst);
        std::string toString() override;
    };

    class Param : Instruction {
    public:
        Var *par;
        Param(Var* par);
        std::string toString() override;
    };

    class Call : Instruction {
    public:
        std::string funLabel;
        int n; //参数个数
        Var* result;
        Call(std::string &funLabel,int n);
        Call(std::string &funLabel,int n,Var* result);
        std::string toString() override;
    };

    class Return: Instruction {
        Var *v;
    public:
        Return(Var * v);
        std::string toString() override;
    };

    class BeginFunc : Instruction {
        //int frameSize;
    public:
        BeginFunc();
        std::string toString() override;
        //void setFrameSize(int numBytesForLocalsAndTemps);
    };

    class EndFunc : Instruction {
    public:
        EndFunc();
        std::string toString() override;
    };

}
