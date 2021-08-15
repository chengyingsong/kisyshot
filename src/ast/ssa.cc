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
                case Binary_op_:
                case Assign_:
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
        std::cout << "reversed post order:";
        for (auto node:order) {
            std::cout << " " << node->label;
        }
        std::cout << std::endl;
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
        for(auto node:nodes) {
            for(auto it = node->begin;std::prev(it)!= node->end;it++){
                if((*it)->getType() == Assign_ && (*it)->src_2->type == LocalVar) {
                    //局部变量赋值语句
                    Var* dst = (*it)->src_2;
                    if(var2block.count(dst) != 0) {
                        var2block[dst].emplace(node);   //把变量dst的赋值节点加入map中
                    } else {
                        std::unordered_set<ControlBlockNode*> v;
                        v.emplace(node);
                        var2block[dst] = v;
                    }
                }
            }
        }
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
                    blockLabels.push_back(e->from->label);
                }
                //插入phi函数
                node->Phis.push_back(new Phi(v.first,blockLabels));
            }
        }
    }


    SSADriver::SSADriver(std::list<Instruction *> &inst) {
        original = inst;
        std::list<Instruction*>::iterator begin;
        for(auto it = inst.begin(); it != inst.end(); it ++){
            if ((*it)->getType() == BeginFunc_)
                begin = it;
            if ((*it)->getType() == EndFunc_)
                graphs.emplace_back(begin, it);
        }
    }

    std::list<Instruction *> SSADriver::transform() {
        for(auto& g:graphs) {
            g.genDominatorTree();    //计算支配树
            g.findFrontiers();      //计算支配边界
            g.getVarMap();         //扫描节点，建立变量--> 赋值block map,即A

/*            for(auto& v:g.var2block){
                std::cout << v.first->getName() << ":" ;
                for(auto& node:v.second){
                    std::cout << node->label << ",";
                }
                std::cout << std::endl;
            }*/

            g.getClosure();       //计算A的闭包

/*            std::cout << "after" << std::endl;
            for(auto& v:g.var2block){
                std::cout << v.first->getName() << ":" ;
                for(auto& node:v.second){
                    std::cout << node->label << ",";
                }
                std::cout << std::endl;
            }*/
            g.insertPhi();        //向闭包中插入phi函数




            for(auto node:g.nodes) {
                std::cout << "dominates: " << node->dominator->label << " -> " << node->label << std::endl;
            }
            for(auto node:g.nodes) {
                if (!node->frontiers.empty()) {
                    std::cout << "frontiers: " << node->label << " -> (";
                    for (int i = 0; i < node->frontiers.size(); ++i) {
                        std::cout << node->frontiers[i]->label;
                        if (i != node->frontiers.size() - 1)
                            std::cout << ", ";
                    }
                    std::cout << ")" << std::endl;
                }
            }

            for(auto node:g.nodes) {
                std::cout << node->label << std::endl;
                if(!node->Phis.empty()){
                    for(auto& phi:node->Phis){
                        std::cout << "Phi(";
                        for(auto& label:phi->blockLabels){
                            std::cout << phi->i->getName()<< "_" << label << ",";
                        }
                        std::cout << ")" <<std::endl;
                    }
                }
            }
        }
        return original;
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