#ifndef ARMS_H
#define ARMS_H

#include "tac.h"
#include "cfg.h"
#include "../context.h"
#include <stdio.h>
#include <vector>

namespace kisyshot::ast {
    class Arms {
    private:
        typedef enum : int {
            r0, r1, r2, r3, r4, r5, r6, r7,
            r8, r9, r10, Fp, ip, sp, lr, pc
        } Register;

        struct RegContents {
            Var * var;
            std::string name;
            bool isDirty;
            bool mutexLock;
            bool canDiscard;
        } regs[16];

        std::vector<Var *> VarStack;
        std::vector<int> regStack;

        std::string opName[11];

        Register rs, rt, rd;
        // 将寄存器映射至变量
        std::map<Register, Var *> regDescriptor;
        // 符号表
        std::shared_ptr<Context> ctx;
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
        // 遇到跳转指令时clean所有的通用寄存器
        void cleanRegForBranch();
        // 遇到call清空所有的通用寄存器
        void cleanRegForCall();
        // 函数结束时clean所有通用寄存器
        void cleanRegForEndFunc();
        // 在regDescriptor中找到var对应的reg
        std::map<Register, Var *>::iterator regDescriptorFind(Var * var);
        // regDescriptor插入
        void regDescriptorInsert(Var * var, Register reg, bool dirty);
        // regDescriptor移除
        void regDescriptorRemove(Var * var, Register reg);
        // 标注reg中的变量可以被替换
        void discardVarInReg(Var * var, Register reg);
        // 从src中读取数据放到寄存器reg中
        void fillReg(Var * src, Register reg);
        // 将寄存器reg中的数据存放到dst中
        void spillReg(Var * dst, Register reg);

    public:
        FILE * fp;
        std::map<Var *, bool> ParamDiscard;
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
        void generateCMP(TokenType opType, Var * src_1, Var * src_2, std::string label);
        void generateBeginFunc(std::string curFunc, int frameSize);
        void generateEndFunc(std::string curFunc, int frameSize);
        void generateReturn(Var * result);
        void generateParam(Var * arg, int num);
        void generateCall(int numVars, std::string label, Var * result, int paramNum);
        void generateHeaders();
        void generateGlobal();
        void generateEnders();
    };
}



#endif