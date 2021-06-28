#include <rang/rang.h>
#include <sstream>
#include <ast/syntax/var_declaration.h>

namespace kisyshot::ast::syntax {

    VarDeclaration::VarDeclaration(const std::shared_ptr<Type> &type, size_t constIndex) {
        _constTokenIndex = constIndex;
        _semiTokenIndex = invalidTokenIndex;
        _type = type;
    }

    void VarDeclaration::add(const std::shared_ptr<VarDefinition> &child) {
        _varDefs.push_back(child);
    }

    void VarDeclaration::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        for (size_t i = 0; i < _varDefs.size(); ++i) {
            syntaxWalker(_varDefs[i], _varDefs.size() == i + 1);
        }
    }

    void VarDeclaration::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::gray << getType()
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::green << _type->toString()
              << rang::fg::reset << " ";
            for (size_t i = 0; i < _varDefs.size(); ++i) {
                s << _varDefs[i]->toString();
                if (i + 1 != _varDefs.size())
                    s << ", ";
            }
            s << std::endl;
        } else {
            s << " " << getType()
              << "<" << this << "> "
              << _type->toString() << " ";
            for (size_t i = 0; i < _varDefs.size(); ++i) {
                s << _varDefs[i]->toString();
                if (i + 1 != _varDefs.size())
                    s << ", ";
            }
            s << std::endl;
        }
    }

    SyntaxType VarDeclaration::getType() {
        return SyntaxType::VariableDeclaration;
    }

    bool VarDeclaration::hasChild() {
        return !_varDefs.empty();
    }

    std::size_t VarDeclaration::start() {
        return _constTokenIndex == invalidTokenIndex ? _varDefs.front()->start() : _constTokenIndex;
    }

    std::size_t VarDeclaration::end() {
        return _varDefs.back()->end();
    }

    void VarDeclaration::setSemiTokenIndex(size_t semiTokenIndex) {
        _semiTokenIndex = semiTokenIndex;
    }

    const std::vector<std::shared_ptr<VarDefinition>> &VarDeclaration::getVarDefs() const {
        return _varDefs;
    }

    size_t VarDeclaration::getConstTokenIndex() const {
        return _constTokenIndex;
    }

    size_t VarDeclaration::getSemiTokenIndex() const {
        return _semiTokenIndex;
    }

    std::shared_ptr<Type> VarDeclaration::getVarType() const {
        return _type;
    }

    void VarDefinition::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        if (_array.empty()) {
            if (_initialValue == nullptr) {
                syntaxWalker(_varName, true);
            } else {
                syntaxWalker(_varName, false);
                syntaxWalker(_initialValue, true);
            }
        } else {
            syntaxWalker(_varName, true);
            bool init = _initialValue != nullptr;
            for (size_t i = 0; i < _array.size(); ++i) {
                syntaxWalker(_array[i], init & (i + 1 == _array.size()));
            }
            if (init) {
                syntaxWalker(_initialValue, true);
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
        s << _varName->toString();
        for (auto &arr:_array) {
            s << '[' << arr->toString() << ']';
        }
        if (_initialValue != nullptr)
            s << " = " << _initialValue->toString();
        return s.str();
    }

    std::size_t VarDefinition::start() {
        return _varName->start();
    }

    std::size_t VarDefinition::end() {
        if (_initialValue != nullptr)
            return _initialValue->end();
        if (_equalTokenIndex != invalidTokenIndex)
            return _equalTokenIndex;
        if (_array.empty())
            return _varName->end();
        return _array.back()->end();
    }

    void VarDefinition::setInitialValue(const std::shared_ptr<Expression> &initialValue) {
        _initialValue = initialValue;
    }

    void VarDefinition::setEqualTokenIndex(size_t equalTokenIndex) {
        _equalTokenIndex = equalTokenIndex;
    }

    VarDefinition::VarDefinition(const std::shared_ptr<Identifier> &name) {
        _varName = name;
        _equalTokenIndex = invalidTokenIndex;
    }

    void VarDefinition::add(const std::shared_ptr<Expression> &child) {
        _array.push_back(child);
    }

    const std::shared_ptr<Identifier> &VarDefinition::getVarName() const {
        return _varName;
    }

    const std::shared_ptr<Expression> &VarDefinition::getInitialValue() const {
        return _initialValue;
    }

    const std::vector<std::shared_ptr<Expression>> &VarDefinition::getArray() const {
        return _array;
    }

    size_t VarDefinition::getEqualTokenIndex() const {
        return _equalTokenIndex;
    }
}