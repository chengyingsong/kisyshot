#include <ast/ssa.h>
namespace kisyshot::ast{
    ControlBlockGraph::ControlBlockGraph(std::list<Instruction *>::iterator beginFunc,
                                         std::list<Instruction *>::iterator endFunc) {
        ControlBlockNode* current = newNode(".entry", beginFunc);
        std::size_t unnamedIndex = 1;
        for (auto it = beginFunc; it != endFunc; it++) {
            switch ((*it)->getType()) {
                case Label_:{
                    current = newNode(((Label*)*it)->label, it);
                    break;
                }
                case GOTO_:
                    sEdges.emplace_back(current->label, ((GOTO*)*it)->label);
                    break;
                case IfZ_:
                    sEdges.emplace_back(current->label, ((IfZ*)*it)->trueLabel);
                    current = newNode(".unnamed" + std::to_string(unnamedIndex), it);
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
            sEdges.emplace_back(nodes.back()->label, node->label);
        }
        names[label] = node;
        nodes.push_back(node);
        return node;
    }

    ControlBlockGraph::~ControlBlockGraph() {
        for (auto edge:edges)
            delete edge;
        for (auto node:nodes)
            delete node;
    }

    void ControlBlockGraph::genDominanceTree() {

    }

    void ControlBlockGraph::findFrontiers() {

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
            g.genDominanceTree();
            g.findFrontiers();
        }
        return original;
    }
}