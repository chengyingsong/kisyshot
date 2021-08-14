#pragma once
#include <list>
#include <vector>
#include <functional>
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
        // 当前node的支配树前节点
        ControlBlockNode* dominator;
        bool traverse;
        void pDfs(const std::function<void(ControlBlockNode*)>& consumer);
    };

    struct ControlBlockGraph {
        std::vector<ControlBlockNode*> nodes;
        std::vector<Edge*> edges;
        std::vector<std::pair<std::string,std::string>> sEdges;
        std::unordered_map<std::string, ControlBlockNode*> names;
        std::unordered_map<ControlBlockNode*, size_t > ids;
        std::unordered_map<ControlBlockNode*, ControlBlockNode* > doms;
        ControlBlockNode* entry;
        ControlBlockGraph(std::list<Instruction *>::iterator beginFunc, std::list<Instruction *>::iterator endFunc);
        ControlBlockNode* newNode(const std::string& label, std::list<Instruction *>::iterator curr);
        ControlBlockNode* intersect(ControlBlockNode* b1, ControlBlockNode* b2);
        void genDominatorTree();
        void findFrontiers();
        void reset();
        std::vector<ControlBlockNode*> postOrder();
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