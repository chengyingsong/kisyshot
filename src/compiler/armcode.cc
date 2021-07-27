#include <compiler/armcode.h>

using namespace kisyshot::compiler;

ArmCodeGenerator::ArmCodeGenerator(std::list<Instruction *> &tacCode) : code(tacCode) {}

void ArmCodeGenerator::generateArmCode() {
    std::list<Instruction *>::iterator p = code.begin();
    std::list<Instruction *>::iterator beginBlock = code.end();
    std::list<Instruction *>::iterator endBlock = code.end();
    while(p != code.end()) {
        // 找到函数起始
        if (dynamic_cast<BeginFunc *>(*p)) {
            beginBlock = p;
            while (p != code.end())
                // 找到函数结束
                if (dynamic_cast<EndFunc *>(*p))
                    break;
            endBlock = p;
            ControlFlowGraph cfg(beginBlock, endBlock);
            cfgList.push_back(cfg);
        }
        p++;
    }
}