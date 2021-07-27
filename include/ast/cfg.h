#ifndef CFG_H
#define CFG_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "tac.h"

using namespace kisyshot::ast;

typedef std::vector<Instruction *> edgeList;
typedef std::list<Instruction *> block;
typedef std::list<Instruction *>::iterator iterator;

namespace kisyshot::ast {
    // 一个函数的控制流图类
    class ControlFlowGraph {
    private:
        iterator first, last;
        // 每条label对应的指令
        std::map<std::string, Instruction *> labelInsMap;
        std::map<Instruction *, edgeList> inEdges;
        std::map<Instruction *, edgeList> outEdges;
        // 将label映射到对应的指令
        void mapLabels();
        // 为指令映射入边和出边
        void mapEdges();
        // 为跳转指令和目标指令间映射边
        void mapEdgesForJump(iterator cur, std::string type);
        // 为u指令和v指令间映射边
        void addEdge(Instruction * u, Instruction * v);
    public:
        ControlFlowGraph(iterator first, iterator last);
        class ForwardFlow;
        class BackwardFlow;
    };

    class ControlFlowGraph::ForwardFlow {
    private:
        ControlFlowGraph &cfg;
    public:
        ForwardFlow(ControlFlowGraph &cfg) : cfg(cfg) {}
        iterator first() {
            return iterator(cfg.first);
        }
        iterator last() {
            return iterator(cfg.last);
        }
        std::map<Instruction *, edgeList> &in() {
            return cfg.inEdges;
        }
        std::map<Instruction *, edgeList> &out() {
            return cfg.outEdges;
        }
    };

    class ControlFlowGraph::BackwardFlow {
    private:
        ControlFlowGraph &cfg;
    public:
        BackwardFlow(ControlFlowGraph &cfg) : cfg(cfg) {}
        iterator first() {
            return iterator(cfg.last);
        }
        iterator last() {
            return iterator(cfg.first);
        }
        std::map<Instruction *, edgeList> &in() {
            return cfg.outEdges;
        }
        std::map<Instruction *, edgeList> &out() {
            return cfg.inEdges;
        }
    };
}

#endif