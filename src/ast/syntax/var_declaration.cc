#include <rang/rang.h>
#include <sstream>
#include <ast/syntax/var_declaration.h>

namespace kisyshot::ast::syntax {
    void VarDeclaration::add(const std::shared_ptr<VarDefinition> &child) {
        varDefs.push_back(child);
    }

    void VarDeclaration::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        for (size_t i = 0; i < varDefs.size(); ++i) {
            syntaxWalker(varDefs[i], varDefs.size() == i + 1);
        }
    }

    void VarDeclaration::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::gray << getType()
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::green << ((constTokenIndex == invalidTokenIndex) ? "" : "const ") << type->toString()
              << rang::fg::reset << " ";
            for (size_t i = 0; i < varDefs.size(); ++i) {
                s << varDefs[i]->toString();
                if (i + 1 != varDefs.size())
                    s << ", ";
            }
            s << std::endl;
        } else {
            s << " " << getType()
              << "<" << this << "> "
              << ((constTokenIndex == invalidTokenIndex) ? "" : "const ") << type->toString() << " ";
            for (size_t i = 0; i < varDefs.size(); ++i) {
                s << varDefs[i]->toString();
                if (i + 1 != varDefs.size())
                    s << ", ";
            }
            s << std::endl;
        }
    }

    SyntaxType VarDeclaration::getType() {
        return SyntaxType::VariableDeclaration;
    }

    bool VarDeclaration::hasChild() {
        return !varDefs.empty();
    }

    std::size_t VarDeclaration::start() {
        return constTokenIndex == invalidTokenIndex ? varDefs.front()->start() : constTokenIndex;
    }

    std::size_t VarDeclaration::end() {
        return varDefs.back()->end();
    }

    void VarDeclaration::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {
        for (auto varDef:varDefs) {
            varDef->genCode(gen, temp);
        }
    }

    void VarDefinition::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        if (dimensionDef.empty()) {
            if (initialValue == nullptr) {
                syntaxWalker(varName, true);
            } else {
                syntaxWalker(varName, false);
                syntaxWalker(initialValue, true);
            }
        } else {
            std::vector<std::shared_ptr<SyntaxNode>> nodes;
            for (auto & i : dimensionDef) {
                if (i != nullptr) {
                    nodes.push_back(i);
                }
            }
            if (initialValue != nullptr)
                nodes.push_back(initialValue);

            syntaxWalker(varName, nodes.empty());
            for (auto & n:nodes) {
                syntaxWalker(n, n == nodes.back());
            }
        }
    }

    void VarDefinition::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::gray << getType()
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::green << type->toString() << ' ' << toString() << rang::fg::reset << std::endl;
        } else {
            s << " " << getType()
              << "<" << this << "> "
              << type->toString() << ' '
              << toString() << std::endl;
        }
    }

    SyntaxType VarDefinition::getType() {
        return SyntaxType::VariableDefinition;
    }

    bool VarDefinition::hasChild() {
        return true;
    }

    std::string VarDefinition::toString() {
        std::stringstream s;
        s << varName->toString();
        for (auto &arr:dimensionDef) {
            s << '[';
            if (arr != nullptr) {
                s << arr->toString();
            }
            s << ']';
        }
        if (initialValue != nullptr)
            s << " = " << initialValue->toString();
        return s.str();
    }

    std::size_t VarDefinition::start() {
        return varName->start();
    }

    std::size_t VarDefinition::end() {
        if (initialValue != nullptr)
            return initialValue->end();
        if (equalTokenIndex != invalidTokenIndex)
            return equalTokenIndex;
        if (dimensionDef.empty())
            return varName->end();
        return dimensionDef.back()->end();
    }

    void VarDefinition::add(const std::shared_ptr<Expression> &child) {
        dimensionDef.push_back(child);
    }

    void VarDefinition::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {
        //std::cout << "defination of " << varName->toString() << std::endl;
        Var* src_1 = new  Var(varName->mangledId);
        gen.name2VarMap[varName->mangledId] = src_1;  //把名字和Var绑定
        if(!dimensionDef.empty()){
            src_1->isArray = true;
        }
        if(initialValue != nullptr && temp == nullptr){  //代表有初始化语句
            if(initialValue->getType() ==SyntaxType::ArrayInitializeExpression) {
                //数组初始化,先设置数组属性
                initialValue->genCode(gen,src_1);
            }else{
                //initialValue有可能是一个数字
                Var * src_2 = initialValue->getVar(gen);
                gen.genAssign(src_2,src_1);
            }
        }
    }
}