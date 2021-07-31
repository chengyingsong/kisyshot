#include <compiler/armcode.h>

using namespace kisyshot::compiler;
using namespace kisyshot::ast;

ArmCodeGenerator::ArmCodeGenerator(std::list<Instruction *> &tacCode) : code(tacCode) {}

void ArmCodeGenerator::generateArmCode() {
    std::list<Instruction *>::iterator p = code.begin();
    std::list<Instruction *>::iterator beginBlock = code.end();
    std::list<Instruction *>::iterator endBlock = code.end();
    // 构造控制流图list
    while(p != code.end()) {
        // 找到函数起始
        if ((*p)->getType() == InstructionType::BeginFunc_) {
            beginBlock = p;
            while (p != code.end()) {
                // 找到函数结束
                if ((*p)->getType() == InstructionType::EndFunc_)
                    break;
                p++;
            }
            endBlock = p;
            ControlFlowGraph cfg(beginBlock, endBlock);
            cfgList.push_back(cfg);
        }
        p++;
    }
/*    std::map<Var *, Instruction *> varList;
    std::map<Var *, Instruction *>::iterator varListIterator;
    // 构造活跃变量list
    for (cfgListIterator = cfgList.begin(); cfgListIterator != cfgList.end(); cfgListIterator++) {
        varList.clear();
        ControlFlowGraph::ForwardFlow flow((*cfgListIterator));
        ControlFlowGraph::ForwardFlow::iterator cfgIterator = flow.first();
        for (; cfgIterator != flow.last(); cfgIterator++) {
            int numVars = (*cfgIterator)->numVars;
            if (numVars >= 1) {
                varListIterator = varList.find((*cfgIterator)->src_1);
                if (varListIterator != varList.end())
                    varList.erase(varListIterator);
                std::cout << (*cfgIterator)->src_1->getName();
                if (((*cfgIterator)->src_1->getName()).find("_temp_") != (*cfgIterator)->src_1->getName().npos) {
                    varList.insert(std::pair<Var *, Instruction *>((*cfgIterator)->src_1, (*cfgIterator)));
                } 
            } 
            if (numVars >= 2) {
                varListIterator = varList.find((*cfgIterator)->src_2);
                if (varListIterator != varList.end())
                    varList.erase(varListIterator);
                if (strstr(((*cfgIterator)->src_2->getName()).c_str(), "_temp_") != NULL) {
                    varList.insert(std::pair<Var *, Instruction *>((*cfgIterator)->src_2, (*cfgIterator)));
                }
            }
            if (numVars >= 3) {
                varListIterator = varList.find((*cfgIterator)->dst);
                if (varListIterator != varList.end())
                    varList.erase(varListIterator);
                if (strstr(((*cfgIterator)->dst->getName()).c_str(), "_temp_") != NULL) {
                    varList.insert(std::pair<Var *, Instruction *>((*cfgIterator)->dst, (*cfgIterator)));
                }
            } 
        }
        liveList.push_back(varList); 
    } */
    Arms arms;
    int block = 0;
    p = code.begin();
    while (p != code.end()) {
        if ((*p)->getType() == InstructionType::Label_)
            arms.generateLabel(((Label *)(*p))->label);
        p++;
    }
}