#ifndef ARMCODE_H
#define ARMCODE_H

#include "../ast/cfg.h"
#include "../ast/arms.h"

// arm汇编代码生成类
namespace kisyshot::compiler {
    class ArmCodeGenerator {
    private:
        std::string curFucLabel;
        int frameSize;
        int paramNum;
        std::shared_ptr<Context> ctx;
        // 三地址码指令序列
        std::list<Instruction *> code;
        // 控制流图列表
        std::list<ControlFlowGraph> cfgList;
        std::list<ControlFlowGraph>::iterator cfgListIterator;
        // 活跃变量表，将每个临时变量映射到最后一次使用该变量的指令
        std::list<std::map<Var *, Instruction *> > liveList;
        std::list<std::map<Var *, Instruction *> >::iterator liveListIterator;
    public:
        ArmCodeGenerator(std::list<Instruction *> &tacCode, const std::shared_ptr<Context> &context);
        void generateSpecial(Instruction * tac, Arms &arms);
        void generateArmCode();
    };
}


#endif