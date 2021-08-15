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
    struct Phi  {
        Var* i;
        std::unordered_map<std::string, Var*> sources;
        std::vector<std::string>  blockLabels;
        Phi(Var* i,std::vector<std::string> blockLabels);
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
        std::vector<ControlBlockNode*> tree;
        std::list<Instruction *> postMoves;
        std::vector<Phi*> Phis;
        bool traverse;
        void pDfs(const std::function<void(ControlBlockNode*)>& consumer);
    };

    struct ControlBlockGraph {
        std::vector<ControlBlockNode*> nodes;
        std::vector<Edge*> edges;
        std::vector<std::pair<std::string,std::string>> sEdges;
        std::unordered_map<std::string, ControlBlockNode*> names;
        std::unordered_map<ControlBlockNode*, size_t > ids;
        std::unordered_map<Var*,std::unordered_set<ControlBlockNode*>> var2block;
        std::unordered_map<std::string, std::vector<Var*>> varStack;
        std::unordered_map<std::string, size_t> counter;
        bool addEdgeOnSwitch;
        ControlBlockNode* entry;
        ControlBlockGraph(std::list<Instruction *>::iterator beginFunc, std::list<Instruction *>::iterator endFunc);
        ControlBlockNode* newNode(const std::string& label, std::list<Instruction *>::iterator curr);
        ControlBlockNode* intersect(ControlBlockNode* b1, ControlBlockNode* b2);
        void genDominatorTree();
        void findFrontiers();
        void getVarMap();
        void getClosure();
        void insertPhi();
        void reset();
        void rename();
        void rename(ControlBlockNode* node);
        Var* cloneVar(Var*);
        std::vector<ControlBlockNode*> postOrder();
        ~ControlBlockGraph();
    };
    // ssa 优化
    struct SSADriver {
        std::vector<ControlBlockGraph> graphs;
        explicit SSADriver(std::list<Instruction *> &inst);
        std::list<Instruction*> transform();  //转换函数
    };
}