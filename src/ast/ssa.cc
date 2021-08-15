#include <ast/ssa.h>
#include <algorithm>
#include <queue>
#include <iostream>
namespace kisyshot::ast{
    ControlBlockGraph::ControlBlockGraph(std::list<Instruction *>::iterator beginFunc,
                                         std::list<Instruction *>::iterator endFunc) {
        ControlBlockNode* current = newNode(".entry", beginFunc);
        entry = current;
        std::size_t unnamedIndex = 1;
        for (auto it = beginFunc; it != endFunc; it++) {
            switch ((*it)->getType()) {
                case Label_:{
                    current = newNode(((Label*)*it)->label, it);
                    break;
                }
                case GOTO_:
                    sEdges.emplace_back(current->label, ((GOTO*)*it)->label);
                    addEdgeOnSwitch = false;
                    break;
                case IfZ_:
                    sEdges.emplace_back(current->label, ((IfZ*)*it)->trueLabel);
                    current = newNode(".unnamed" + std::to_string(unnamedIndex++), it);
                    break;
                case CMP_:
                    // TODO push cmp edges
                    sEdges.emplace_back(current->label, ((CMP*)*it)->label);
                    break;
                case Assign_:
                    if ((*it)->src_2->type == LocalVar && !(*it)->src_2->isParam)
                        var2block[(*it)->src_2].emplace(current);
                    break;
                case Binary_op_:
                    if ((*it)->dst->type == LocalVar && !(*it)->dst->isParam)
                        var2block[(*it)->dst].emplace(current);
                    break;
                case Load_:
                case Store_:
                case Param_:
                case Call_:
                case Return_:
                case BeginFunc_:
                case EndFunc_:
                    break;
            }
        }

        nodes.back()->end = endFunc;
        // init edges;
        for(auto &[from, to] : sEdges){
            ControlBlockNode* fromNode = names[from], *toNode = names[to];
            Edge* edge = new Edge(fromNode, toNode);
            edges.push_back(edge);
            fromNode->out.push_back(edge);
            toNode->in.push_back(edge);
        }
    }

    ControlBlockNode *ControlBlockGraph::newNode(const std::string &label, std::list<Instruction *>::iterator curr) {
        auto node = new ControlBlockNode();
        node->label = label;
        node->begin = curr;
        if (!nodes.empty()) {
            nodes.back()->end = std::prev(curr);
            if (addEdgeOnSwitch)
                sEdges.emplace_back(nodes.back()->label, node->label);
        }
        names[label] = node;
        ids[node] = nodes.size();
        nodes.push_back(node);
        addEdgeOnSwitch = true;
        return node;
    }

    ControlBlockGraph::~ControlBlockGraph() {
        for (auto edge:edges)
            delete edge;
        for (auto node:nodes)
            delete node;
    }

    void ControlBlockGraph::genDominatorTree() {
        // TODO check : https://blog.csdn.net/dashuniuniu/article/details/52224882
        for (auto node:nodes) {
            node->dominator = nullptr;
        }
        bool changed = true;
        auto order = postOrder();
        order.pop_back();
        std::reverse(order.begin(), order.end());
        reset();
        entry->traverse = true;
        entry->dominator = entry;

        while (changed){
            changed = false;
            for(auto b:order){
                Edge* newIDomE;
                ControlBlockNode* newIDom;
                for(auto e:b->in){
                    if (e->from->traverse){
                        newIDomE = e;
                        newIDom = e->from;
                        break;
                    }
                }

                for(auto e:b->in){
                    if (e != newIDomE){
                        if(e->from->dominator != nullptr){
                            newIDom = intersect(e->from, newIDom);
                        }
                    }
                }
                if(b->dominator != newIDom){
                    b->dominator = newIDom;
                    b->traverse = true;
                    changed = true;
                }
            }
        }
        // gen dominator tree

        for (auto node:nodes) {
            if (node->dominator != node){
                node->dominator->tree.push_back(node);
            }
        }
    }

    void ControlBlockGraph::findFrontiers() {
        for(auto node :nodes){
            if (node->in.size() >= 2){
                for(auto e: node->in){
                    auto runner = e->from;
                    while (runner != node->dominator){
                        runner->frontiers.push_back(node);
                        runner = runner->dominator;
                    }
                }
            }
        }
    }

    std::vector<ControlBlockNode *> ControlBlockGraph::postOrder() {
        std::vector<ControlBlockNode *> result;
        reset();
        entry->pDfs([&result] (ControlBlockNode* node)->void {
            result.push_back(node);
        });
        return result;
    }

    void ControlBlockGraph::reset() {
        for(auto node:nodes)
            node->traverse = false;
    }

    ControlBlockNode *ControlBlockGraph::intersect(ControlBlockNode *b1, ControlBlockNode *b2) {
        size_t f1 = ids[b1], f2 = ids[b2];
        // 文章中entry id最大所以这里方向反过来
        while (f1 != f2){
            while (f1 > f2)
                f1 = ids[nodes[f1]->dominator];
            while (f2 > f1)
                f2 = ids[nodes[f2]->dominator];
        }
        return nodes[f1];
    }

    void ControlBlockGraph::getVarMap() {
        //扫描节点，建立变量--> 赋值block map
//        for(auto node:nodes) {
//            for(auto it = node->begin;std::prev(it)!= node->end;it++){
//                if((*it)->getType() == Assign_ && (*it)->src_2->type == LocalVar) {
//                    //局部变量赋值语句
//                    Var* dst = (*it)->src_2;
//                    if(var2block.count(dst) != 0) {
//                        var2block[dst].emplace(node);   //把变量dst的赋值节点加入map中
//                    } else {
//                        std::unordered_set<ControlBlockNode*> v;
//                        v.emplace(node);
//                        var2block[dst] = v;
//                    }
//                }
//            }
//        }
    }

    void ControlBlockGraph::getClosure() {
        //计算闭包
        for(auto& v:var2block) {
            std::unordered_set<ControlBlockNode*> an1;
            std::unordered_set<ControlBlockNode*> an2;
            for(auto& node:v.second){
                for(auto& frontier:node->frontiers)
                    an1.emplace(frontier);
            } //an1=DF[A]  B
            while(an1 != an2){
                an2 = an1;
                an1.clear();
                for(auto& node:an2){
                    v.second.emplace(node);
                }
                for(auto& node:v.second){
                    for(auto& frontier:node->frontiers)
                        an1.emplace(frontier);
                }
            }
            v.second = an2;
        }
    }

    void ControlBlockGraph::insertPhi() {
        //在闭包中插入phi函数
        for(auto& v:var2block) {
            //对每一个变量查询闭包
            for(auto& node:v.second){
                //对每一个闭包中的node查看入边
                std::vector<std::string> blockLabels;
                for(auto& e:node->in){
                    if(var2block[v.first].count(e->from) != 0)
                        blockLabels.push_back(e->from->label);
                }
                if(blockLabels.size() > 1)
                    node->Phis.push_back(new Phi(v.first,blockLabels));
            }
        }
    }

    Var *ControlBlockGraph::cloneVar(Var * dst) {
        Var* clone = new Var();
        clone->type = TempVar;
        clone->value = dst->value;
        clone->index = ++counter[dst->variableName];
        clone->isArray = dst->isArray;
        clone->isParam = dst->isParam;
        clone->s = dst->s;
        clone->variableName = dst->variableName;
        return clone;
    }

    void ControlBlockGraph::rename(ControlBlockNode *node) {
        if (node->traverse)
            return;
        node->traverse = true;
        std::unordered_map<std::string, size_t> originalStackSize;
        for(auto &s: varStack){
            originalStackSize.emplace(s.first,s.second.size());
        }
        for(auto phi:node->Phis){
            phi->i = cloneVar(phi->i);
            varStack[phi->i->variableName].push_back(phi->i);
        }
        for(auto it = node->begin; std::prev(it) != node->end; it++){
            auto instr = *it;
            switch (instr->getType()) {
                case Binary_op_:
                    if (instr->src_1->type == LocalVar && !instr->src_1->isParam)
                        instr->src_1 = varStack[instr->src_1->variableName].back();
                    if (instr->src_2->type == LocalVar && !instr->src_2->isParam)
                        instr->src_2 = varStack[instr->src_2->variableName].back();
                    if (instr->dst->type == LocalVar && !instr->dst->isParam) {
                        instr->dst = cloneVar(instr->dst);
                        varStack[instr->dst->variableName].push_back(instr->dst);
                    }
                    break;
                case Assign_:
                    if (instr->src_1->type == LocalVar && !instr->src_1->isParam)
                        instr->src_1 = varStack[instr->src_1->variableName].back();
                    if (instr->src_2->type == LocalVar && !instr->src_2->isParam) {
                        instr->src_2 = cloneVar(instr->src_2);
                        varStack[instr->src_2->variableName].push_back(instr->src_2);
                    }
                    break;
                case Load_:
                case Store_:
                case BeginFunc_:
                    break;
                default:
                    if (instr->src_1 != nullptr && instr->src_1->type == LocalVar && !instr->src_1->isParam)
                        instr->src_1 = varStack[instr->src_1->variableName].back();
                    if (instr->src_2 != nullptr && instr->src_2->type == LocalVar && !instr->src_2->isParam)
                        instr->src_2 = varStack[instr->src_2->variableName].back();
                    break;
            }
        }

        for(auto e:node->out) {
            for(auto phi:e->to->Phis){
                if (std::find(phi->blockLabels.begin(), phi->blockLabels.end(), node->label) != phi->blockLabels.end()){
                    phi->sources[node->label] = varStack[phi->i->variableName].back();
                }
            }
        }

        for(auto e:node->out){
            rename(e->to);
        }
        for(auto &s: varStack){
            varStack[s.first].resize(originalStackSize[s.first]);
        }
    }

    void ControlBlockGraph::rename() {
        reset();
        rename(entry);
    }


    SSADriver::SSADriver(std::list<Instruction *> &inst) {
        std::list<Instruction*>::iterator begin;
        for(auto it = inst.begin(); it != inst.end(); it ++){
            if ((*it)->getType() == BeginFunc_)
                begin = it;
            if ((*it)->getType() == EndFunc_)
                graphs.emplace_back(new ControlBlockGraph(begin, it));
        }
    }

    std::list<Instruction *> SSADriver::transform() {

        std::list<Instruction*> transformed;
        for(auto& g:graphs) {
            g->genDominatorTree();    //计算支配树
            g->findFrontiers();      //计算支配边界
            g->getVarMap();         //扫描节点，建立变量--> 赋值block map,即A
            g->getClosure();       //计算A的闭包
            g->insertPhi();        //向闭包中插入phi函数
            g->rename();

            for(auto node:g->nodes){
                // SSA resolve
                for(auto phi:node->Phis){
                    for(auto& [bName, var]:phi->sources){
                        g->names[bName]->postMoves.push_back((Instruction*)new Assign(var, phi->i));
                    }
                }
            }
            // add function label
            transformed.push_back(*std::prev(g->entry->begin));
            for(auto node:g->nodes) {
                for (auto it = node->begin; it != node->end; it++) {
                    transformed.push_back(*it);
                }
                if ((*node->end)->getType() == GOTO_){
                    transformed.insert(transformed.end(), node->postMoves.begin(), node->postMoves.end());
                    transformed.push_back(*node->end);
                } else {
                    transformed.push_back(*node->end);
                    transformed.insert(transformed.end(), node->postMoves.begin(), node->postMoves.end());
                }
            }


//            for(auto node:g.nodes) {
//                std::cout << "dominates: " << node->dominator->label << " -> " << node->label << std::endl;
//            }
//            for(auto node:g.nodes) {
//                if (!node->frontiers.empty()) {
//                    std::cout << "frontiers: " << node->label << " -> (";
//                    for (int i = 0; i < node->frontiers.size(); ++i) {
//                        std::cout << node->frontiers[i]->label;
//                        if (i != node->frontiers.size() - 1)
//                            std::cout << ", ";
//                    }
//                    std::cout << ")" << std::endl;
//                }
//            }
//
//            for(auto node:g.nodes) {
//                std::cout << node->label << std::endl;
//                if(!node->Phis.empty()){
//                    for(auto& phi:node->Phis){
//                        std::cout << "Phi(";
//                        for(auto& label:phi->blockLabels){
//                            std::cout << phi->i->getName()<< "_" << label << ",";
//                        }
//                        std::cout << ")" <<std::endl;
//                    }
//                }
//            }
            std::cout << "SSA IR of function " + ((GOTO*)(*std::prev(g->entry->begin)))->label << std::endl;
            for(auto node: g->nodes) {
                for (auto it = node->begin; std::prev(it) != node->end; it++) {
                    if ((*it)->getType() == Label_) {
                        std::cout << (*it)->toString() << std::endl;
                        for (auto phi:g->names[((Label *) *it)->label]->Phis) {
                            std::cout << "\t" << phi->i->getName() << "= φ(";
                            std::string phiContent;
                            for (auto &pair:phi->sources) {
                                phiContent += pair.second->getName() + ",";
                            }
                            phiContent.pop_back();
                            std::cout << phiContent << ")" << std::endl;
                        }
                    } else{
                        std::cout << "\t" << (*it)->toString() << std::endl;
                    }
                }
            }
        }

        std::cout << "transformed ir:" << std::endl;
        for(auto i:transformed) {
            if (i->getType() == Label_)
                std::cout << i->toString() << std::endl;
            else
                std::cout << "\t" << i->toString() << std::endl;

        }
        return transformed;
    }

    void ControlBlockNode::pDfs(const std::function<void(ControlBlockNode *)> &consumer) {
        traverse = true;
        for(auto e:out){
            if (!e->to->traverse)
                e->to->pDfs(consumer);
        }
        consumer(this);
    }

    Phi::Phi(Var *i, std::vector<std::string> blockLabels) :i(i),blockLabels(blockLabels){}
}