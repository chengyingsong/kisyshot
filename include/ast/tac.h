#pragma once

#include <string>


namespace kisyshot::ast {
    enum VarType{
        ConstVar,
        GlobalVar,
        LocalVar,
        TempVar,
        StringVar,
    };

    class Var {
        //TODO: 完成四种类型的变量和offset设置
        /*
        Var是表示四元式里，操作数的数据结构。操作数有临时变量类型，全局变量类型和整型
         变量有重整名,字符串是当作字符串常量处理。
        关于全局变量和局部变量的问题，在Var中判断类型后，可以使用getname方法得到重整名
         ，然后在context中map[key]得到符号表项，一般来说是VarDefination节点，其中有属性
         offset和width，都是公开的。
         例子：
        if(v->type == VarType::LocalVar){
            ctx->symbols[v->getName()]->offset  (建议可以在你的文件中写一个这个方法

         }
        */
    public:
        std::string variableName;  //变量名，可以是变量名或者变量重整名
        int value;
        std::string  s;
        VarType type;
        bool isArray = false;  //如果是数组，在初始化类Var的时候设置isArray为true。

        //传入一个变量名建立一个Var对象，需要判断是否是全局变量
        Var(std::string variableName);

        //传入常量
        Var(int value);


        //常数返回数值转字符串，其他类型返回名字
        std::string getName();
    };

    enum InstructionType{
        Binary_op_,
        GOTO_,
        Label_,
        IfZ_,
        Assign_,
        Load_,
        Store_,
        Param_,
        Call_,
        Return_,
        BeginFunc_,
        EndFunc_
    };

    class Instruction {
        //一个基类，表示各种指令
    public:
        //返回指令的三地址码
        virtual std::string toString() = 0;
        virtual InstructionType getType() = 0;
        //void generate();

        Var*  src_1;
        Var*  src_2;
        Var*  dst;
        int numVars;
        Instruction();
        Instruction(Var* src_1);
        Instruction(Var* src_1,Var* src_2);
        Instruction(Var* src_1,Var* src_2,Var* dst);
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
        InstructionType getType() override;

        OpCode code;   //运算符号类型
        //Var *src_1,*src_2,*dst;
        //一个构造器
        Binary_op(OpCode c,Var *src_1,Var *src_2,Var *Dst);
    };

    //中间代码生成后无 逻辑表达式，也就是没有A && B


    class GOTO :Instruction {
    public:
        std::string label;
        GOTO(std::string &label);
        std::string toString() override;
        InstructionType getType() override;
    };


    class Label :Instruction {
        std::string label;
    public:
        Label(std::string &label);
        std::string toString() override;
        InstructionType getType() override;
    };

    class IfZ :Instruction {
    public:
        //如果conditon的值为0则跳转到label处
        std::string trueLabel;
        IfZ(Var* condition,std::string &trueLabel);
        std::string toString() override;
        InstructionType getType() override;
    };

    class Assign : Instruction {
        //Assign是两个变量之间的赋值
    public:
        Assign(Var* t1,Var *t2);
        std::string toString() override;
        InstructionType getType() override;
    };


    class Load : Instruction {
        //Load是把数组的值取出来
    public:
        Load(Var *src_1,Var* src_2,Var* dst);
        std::string toString() override;
        InstructionType getType() override;
    };


    class Store : Instruction {
        //Store是把值存入数组中
    public:
        Store(Var *src_1,Var* src_2,Var *dst);
        std::string toString() override;
        InstructionType getType() override;
    };

    class Param : Instruction {
    public:
        Param(Var* par);
        std::string toString() override;
        InstructionType getType() override;
    };

    class Call : Instruction {
    public:
        std::string funLabel;
        int n; //参数个数
        Call(std::string &funLabel,int n);
        Call(std::string &funLabel,int n,Var* result);
        std::string toString() override;
        InstructionType getType() override;
    };

    class Return: Instruction {
    public:
        Return(Var * v);
        std::string toString() override;
        InstructionType getType() override;
    };

    class BeginFunc : Instruction {
        int frameSize;
    public:
        BeginFunc();
        std::string toString() override;
        InstructionType getType() override;
        //TODO: 设置栈帧
        void setFrameSize(int numBytesForLocalsAndTemps);
    };

    class EndFunc : Instruction {
    public:
        EndFunc();
        std::string toString() override;
        InstructionType getType() override;
    };

}
