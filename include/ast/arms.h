#ifndef ARMS_H
#define ARMS_H

#include "tac.h"
#include "cfg.h"
#include "../context.h"

namespace kisyshot::ast {
    class Arms {
    private:
        typedef enum {
            r0, r1, r2, r3, r4, r5, r6, r7,
            r8, r9, r10, r11, r12, sp, lr, pc
        } Register;

        struct RegContents {
            Var * var;
            std::string name;
            bool isDirty;
            bool mutexLock;
            bool canDiscard;
        } regs[16];

        Register rs, rt, rd;
        // 将寄存器映射至变量
        std::map<Register, Var *> regDescriptor;
        // 符号表
        std::shared_ptr<Context> ctx;

        // 获得基址
        std::string getBase(Var * var);
        // 获得偏移值
        int getOffset(Var * var);
        // 比较两变量是否相同
        bool varsAreSame(Var * var1, Var * var2);
        // 找到存放var的寄存器
        int findRegForVar(Var * var);
        // 找到一个干净的寄存器
        int findCleanReg();
        // 随机选择一个dirty的寄存器
        int selectRandomReg();
        // 为var挑选一个可存放的寄存器
        int pickRegForVar(Var * var);
        // 获得reg中存放的变量
        Var * getRegContents(Register reg);
        // clean寄存器reg
        void cleanReg(Register reg);
        // 遇到跳转指令或标号时clean所有的通用寄存器（r0~r12，r7除外）
        void cleanRegForBranch();
        // 在regDescriptor中找到var对应的reg
        std::map<Register, Var *>::iterator regDescriptorFind(Var * var);
        // regDescriptor插入
        void regDescriptorInsert(Var * var, Register reg);
        // regDescriptor移除
        void regDescriptorRemove(Var * var, Register reg);
        // regDescriptor更新
        void regDescriptorUpdate(Var * var);
        // 标注reg中的变量可以被替换
        void discardVarInReg(Var * var, Register reg);
        // 从src中读取数据放到寄存器reg中
        void fillReg(Var * src, Register reg);
        // 将寄存器reg中的数据存放到dst中
        void spillReg(Var * dst, Register reg);

    public:
        Arms(const std::shared_ptr<Context> &context);
        void generateDiscardVar(Var * var);
        void generateAssignConst(Var * dst, Var * src);
        void generateAssign(Var * dst, Var * src);
        void generateLoad(Var * dst, Var * src, Var * offset);
        void generateStore(Var * dst, Var * offset, Var * src);
        void generateBinaryOP(Binary_op::OpCode op, Var * dst, Var * src_1, Var * src_2);
        void generateLabel(std::string label);
        void generateGOTO(std::string label);
        void generateIfZ(Var * test, std::string label);
        void generateBeginFunc(std::string curFunc, int frameSize);
        void generateEndFunc(std::string curFunc, int frameSize);
        void generateReturn();
        void generateParam(Var * arg, int num);
        void generateCall(int numVars, std::string label, Var * result);
        void generateHeaders();
        void generateGlobal();
    };
}



#endif