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
            varDef->genCode(gen, nullptr);
        }
    }

    void VarDefinition::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        if (array.empty()) {
            if (initialValue == nullptr) {
                syntaxWalker(varName, true);
            } else {
                syntaxWalker(varName, false);
                syntaxWalker(initialValue, true);
            }
        } else {
            syntaxWalker(varName, true);
            bool init = initialValue != nullptr;
            for (size_t i = 0; i < array.size(); ++i) {
                if (array[i] != nullptr) {
                    syntaxWalker(array[i], init & (i + 1 == array.size()));
                }
            }
            if (init) {
                syntaxWalker(initialValue, true);
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
        for (auto &arr:array) {
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
        if (array.empty())
            return varName->end();
        return array.back()->end();
    }

    void VarDefinition::add(const std::shared_ptr<Expression> &child) {
        array.push_back(child);
    }

    void VarDefinition::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {
        //std::cout << "defination of " << varName->toString() << std::endl;
        Var* src_1 = new  Var(varName->toString());
        gen.name2VarMap[varName->toString()] = src_1;  //把名字和Var绑定
        if(initialValue != nullptr){
            //initialValue有可能是一个数字
            Var * src_2 = initialValue->getVar(gen);
            if(src_1->isGlobal() && src_2->isGlobal()){
                //两边都是global,src_1是左值
                Var *t = gen.newTempVar();
                gen.genLoad(src_2,t);
                gen.genStore(t,src_1);
            }else{
                if(src_1->isGlobal()){
                    gen.genStore(src_2,src_1);
                }else{
                    if(src_2->isGlobal()){
                        gen.genLoad(src_2,src_1);
                    }else{
                        gen.genAssign(src_2,src_1);
                    }
                }
            }
        }
    }
}