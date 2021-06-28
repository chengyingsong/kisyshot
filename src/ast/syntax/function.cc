#include <sstream>
#include <memory>
#include <rang/rang.h>
#include <ast/syntax/function.h>

namespace kisyshot::ast::syntax {

    Function::Function(const std::shared_ptr<Type> &returnType, const std::shared_ptr<Identifier> &name,
                       const std::shared_ptr<ParamList> &params, const std::shared_ptr<Statement> &body) {
        _returnType = returnType;
        _name = name;
        _params = params;
        _body = body;
        _lParenIndex = invalidTokenIndex;
        _rParenIndex = invalidTokenIndex;
    }

    void Function::setLParenIndex(std::size_t lParenIndex) {
        _lParenIndex = lParenIndex;
    }

    void Function::setRParenIndex(std::size_t rParenIndex) {
        _rParenIndex = rParenIndex;
    }

    void Function::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(_name, false);
        syntaxWalker(_returnType, false);
        syntaxWalker(_params, false);
        syntaxWalker(_body, true);
    }

    void Function::writeCurrentInfo(std::ostream &s) {
        std::ostringstream methodData;
        methodData << "'" << _returnType->toString() << " " << _name->toString();

        methodData << "(";
        if (_params->hasChild()) {
            _params->forEachChild([&methodData](const std::weak_ptr<SyntaxNode> &n, bool isLast) {
                auto para = std::static_pointer_cast<ParamDeclaration>(n.lock());
                methodData << para->getParamType().lock()->toString()
                           << (isLast ? ")'" : ", ");
            });
        } else {
            methodData << ")'";
        }
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::gray << "Function "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::green << methodData.str()
              << rang::fg::reset << std::endl;
        } else {
            s << "Function "
              << "<" << this << "> "
              << methodData.str() << std::endl;
        }
    }

    bool Function::hasChild() {
        return true;
    }

    SyntaxType Function::getType() {
        return SyntaxType::Function;
    }

    std::size_t Function::start() {
        return _returnType->start();
    }

    std::size_t Function::end() {
        if (_body->getType() == SyntaxType::BlockStatement)
            return _body->end();
        if (_body->end() != invalidTokenIndex)
            return _body->end();
        if (_rParenIndex != invalidTokenIndex)
            return _rParenIndex;
        if (_params->hasChild())
            return _params->end();
        if (_lParenIndex != invalidTokenIndex)
            return _lParenIndex;
        return _name->end();
    }

    const std::shared_ptr<Type> &Function::getReturnType() const {
        return _returnType;
    }

    const std::shared_ptr<Identifier> &Function::getName() const {
        return _name;
    }

    const std::shared_ptr<ParamList> &Function::getParams() const {
        return _params;
    }

    const std::shared_ptr<Statement> &Function::getBody() const {
        return _body;
    }

    size_t Function::getLParenIndex() const {
        return _lParenIndex;
    }

    size_t Function::getRParenIndex() const {
        return _rParenIndex;
    }
}