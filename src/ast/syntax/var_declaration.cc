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
              << rang::fg::green << type->toString()
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
              << type->toString() << " ";
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
                syntaxWalker(array[i], init & (i + 1 == array.size()));
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
              << rang::fg::reset << toString() << std::endl;
        } else {
            s << " " << getType()
              << "<" << this << "> "
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
            s << '[' << arr->toString() << ']';
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
}