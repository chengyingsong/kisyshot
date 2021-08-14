#pragma once
#include <list>
#include <vector>
#include "tac.h"
namespace kisyshot::ast {
    struct ControlBlockNode;
    struct Edge {
        ControlBlockNode *from, *to;
        Edge(ControlBlockNode *from, ControlBlockNode *to):from(from),to(to){};
    };

    struct ControlBlockNode {
        // 块标签
        std::string label;
        // 块指令
        std::list<Instruction *>::iterator begin, end;
        // 入边、出边
        std::vector<Edge *> in, out;
        // 当前node的支配边界
        std::vector<ControlBlockNode *> frontiers;
        // 当前node的支配树
        std::vector<ControlBlockNode *> dominance;
    };

    struct ControlBlockGraph {
        std::vector<ControlBlockNode*> nodes;
        std::vector<Edge*> edges;
        std::vector<std::pair<std::string,std::string>> sEdges;
        std::unordered_map<std::string, ControlBlockNode*> names;
        ControlBlockGraph(std::list<Instruction *>::iterator beginFunc, std::list<Instruction *>::iterator endFunc);
        ControlBlockNode* newNode(const std::string& label, std::list<Instruction *>::iterator curr);
        void genDominanceTree();
        void findFrontiers();
        ~ControlBlockGraph();
    };
    // ssa 优化
    struct SSADriver {
        std::vector<ControlBlockGraph> graphs;
        // TODO remove after ssa form ir generated
        std::list<Instruction*> original;
        explicit SSADriver(std::list<Instruction *> &inst);
        std::list<Instruction*> transform();
    };
}