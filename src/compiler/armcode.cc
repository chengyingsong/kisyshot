#include <compiler/armcode.h>

using namespace kisyshot::compiler;
using namespace kisyshot::ast;

ArmCodeGenerator::ArmCodeGenerator(std::list<Instruction *> &tacCode, const std::shared_ptr<Context> &context) : code(tacCode) {
    ctx = context;
}

void ArmCodeGenerator::generateSpecial(Instruction * tac, Arms &arms) {
    if (tac->getType() == InstructionType::Assign_) {
        std::cout << tac->src_1->getName() << " " << tac->src_1->type << std::endl;
        std::cout << tac->src_2->getName() << " " << tac->src_2->type << std::endl;
        arms.generateAssign(tac->src_2, tac->src_1);
    }

    if (tac->getType() == InstructionType::Binary_op_)
        arms.generateBinaryOP(((Binary_op *)tac)->code, tac->dst, tac->src_1, tac->src_2);
    if (tac->getType() == InstructionType::Call_)
        arms.generateCall(tac->numVars, ((Call *)tac)->funLabel, tac->src_1);
    if (tac->getType() == InstructionType::GOTO_)
        arms.generateGOTO(((Label *)tac)->label);
    if (tac->getType() == InstructionType::IfZ_)
        arms.generateIfZ(tac->src_1, ((IfZ *)tac)->trueLabel);
    if (tac->getType() == InstructionType::Label_)
        arms.generateLabel(((Label *)tac)->label);
    if (tac->getType() == InstructionType::Load_)
        arms.generateLoad(tac->dst, tac->src_1, tac->src_2);
    if (tac->getType() == InstructionType::Store_)
        arms.generateStore(tac->src_2, tac->dst, tac->src_1);
    if (tac->getType() == InstructionType::Param_)
        arms.generateParam(tac->src_1, paramNum);
    if (tac->getType() == InstructionType::BeginFunc_)
        arms.generateBeginFunc(curFucLabel, frameSize);
    if (tac->getType() == InstructionType::Return_)
        arms.generateReturn(tac->src_1);
    if (tac->getType() == InstructionType::EndFunc_)
        arms.generateEndFunc(curFucLabel, frameSize);
}

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
    std::map<Var *, Instruction *> varList;
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
                if (((*cfgIterator)->src_1->getName()).find("_temp_") != (*cfgIterator)->src_1->getName().npos) {
                    varList.insert(std::pair<Var *, Instruction *>((*cfgIterator)->src_1, (*cfgIterator)));
                } 
            } 
            if (numVars >= 2) {
                varListIterator = varList.find((*cfgIterator)->src_2);
                if (varListIterator != varList.end())
                    varList.erase(varListIterator);
                if (((*cfgIterator)->src_2->getName()).find("_temp_") != (*cfgIterator)->src_1->getName().npos) {
                    varList.insert(std::pair<Var *, Instruction *>((*cfgIterator)->src_2, (*cfgIterator)));
                }
            }
            if (numVars >= 3) {
                varListIterator = varList.find((*cfgIterator)->dst);
                if (varListIterator != varList.end())
                    varList.erase(varListIterator);
                if (((*cfgIterator)->dst->getName()).find("_temp_") != (*cfgIterator)->src_1->getName().npos) {
                    varList.insert(std::pair<Var *, Instruction *>((*cfgIterator)->dst, (*cfgIterator)));
                }
            } 
        }
        liveList.push_back(varList); 
    } 
    int block = 0;
    Arms arms(ctx);
    p = code.begin();
    arms.generateHeaders();
    arms.generateGlobal();
    paramNum = 0;
    while (p != code.end()) {
        if ((*p)->getType() == InstructionType::BeginFunc_) {
            beginBlock = p;
            frameSize = ((BeginFunc *)(*p))->frameSize;
            while (p != code.end()) {
                if ((*p)->getType() == InstructionType::EndFunc_)
                    break;
                p++;
            }
            endBlock = p;
            block++;
            if (block == 1)
                liveListIterator = liveList.begin();
            else
                liveListIterator++;
            for (p = beginBlock; p != endBlock; p++) {
                generateSpecial(*p, arms);
                for (varListIterator = (*liveListIterator).begin(); varListIterator != (*liveListIterator).end(); varListIterator++)
                    if ((*p) == (*varListIterator).second)
                        arms.generateDiscardVar((*varListIterator).first);

            }
        }
        if ((*p)->getType() == InstructionType::Label_)
            curFucLabel = ((Label *)(*p))->label;
        if ((*p)->getType() == InstructionType::Param_)
            paramNum++;
        else
            paramNum = 0;
        generateSpecial(*p, arms);
        p++;
    }
}