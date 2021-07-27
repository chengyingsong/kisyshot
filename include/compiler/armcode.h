#ifndef ARMCODE_H
#define ARMCODE_H

#include "../ast/cfg.h"

// arm汇编代码生成类
namespace kisyshot::compiler {
    class ArmCodeGenerator {
    private:
        // 三地址码指令序列
        std::list<Instruction *> code;
        // 控制流图列表
        std::list<ControlFlowGraph> cfgList;
        std::list<ControlFlowGraph>::iterator cfgListIterator;
    public:
        ArmCodeGenerator(std::list<Instruction *> &tacCode);
        void generateArmCode();
    };
}


#endif