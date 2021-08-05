#include <ast/cfg.h>

ControlFlowGraph::ControlFlowGraph(iterator first, iterator last) : first(first), last(last) {
    mapLabels();
    mapEdges();
}

void ControlFlowGraph::mapLabels() {
    for (iterator p = first; p != last; p++) {
        Label * label = dynamic_cast<Label *>(*p);
        if (label)
            labelInsMap[label->label] = *p;
    }
}

void ControlFlowGraph::addEdge(Instruction * u, Instruction * v) {
    inEdges[v].push_back(u);
    outEdges[u].push_back(v);
}

void ControlFlowGraph::mapEdgesForJump(iterator cur, std::string type) {
    std::string label;
    Instruction * instruction;
    if (type == "Call")
        addEdge(*cur, *cur);
    else if (type == "GOTO") {
        label = dynamic_cast<GOTO *>(*cur)->label;
        instruction = labelInsMap.at(label);
        addEdge(*cur, instruction);
    } else if (type == "IfZ") {
        label = dynamic_cast<IfZ *>(*cur)->trueLabel;
        instruction = labelInsMap.at(label);
        addEdge(*cur, instruction);
    }
}

void ControlFlowGraph::mapEdges() {
    std::string type;
    for (iterator cur = first; cur != last; cur++) {
        iterator next = cur;
        next++;
        if (dynamic_cast<Return *>(*cur)) {
            next = first;
            addEdge(*cur, *next);
        } else if (dynamic_cast<GOTO *>(*cur)) {
            type = "GOTO";
            mapEdgesForJump(cur, type);
        } else if (dynamic_cast<IfZ *>(*cur)) {
            type = "IfZ";
            mapEdgesForJump(cur, type);
        } else if (dynamic_cast<Call *>(*cur)) {
            type = "Call";
            mapEdgesForJump(cur, type);
        }
    }
}