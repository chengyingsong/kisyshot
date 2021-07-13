#include <sstream>
#include <memory>
#include <rang/rang.h>
#include <ast/syntax/function.h>

namespace kisyshot::ast::syntax {

    Function::Function(const std::shared_ptr<Type> &returnType, const std::shared_ptr<Identifier> &name,
                       const std::shared_ptr<ParamList> &params, const std::shared_ptr<Statement> &body) {
        this->returnType = returnType;
        this->name = name;
        this->params = params;
        this->body = body;
        this->lParenIndex = invalidTokenIndex;
        this->rParenIndex = invalidTokenIndex;
    }

    void Function::setLParenIndex(std::size_t lParenIndex) {
        lParenIndex = lParenIndex;
    }

    void Function::setRParenIndex(std::size_t rParenIndex) {
        rParenIndex = rParenIndex;
    }

    void Function::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(name, false);
        syntaxWalker(returnType, false);
        syntaxWalker(params, false);
        syntaxWalker(body, true);
    }

    void Function::writeCurrentInfo(std::ostream &s) {
        std::ostringstream methodData;
        methodData << "'" << returnType->toString() << " " << name->toString();

        methodData << "(";
        if (params->hasChild()) {
            params->forEachChild([&methodData](const std::weak_ptr<SyntaxNode> &n, bool isLast) {
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
        return returnType->start();
    }

    std::size_t Function::end() {
        if (body->getType() == SyntaxType::BlockStatement)
            return body->end();
        if (body->end() != invalidTokenIndex)
            return body->end();
        if (rParenIndex != invalidTokenIndex)
            return rParenIndex;
        if (params->hasChild())
            return params->end();
        if (lParenIndex != invalidTokenIndex)
            return lParenIndex;
        return name->end();
    }

    const std::shared_ptr<Type> &Function::getReturnType() const {
        return returnType;
    }

    const std::shared_ptr<Identifier> &Function::getName() const {
        return name;
    }

    const std::shared_ptr<ParamList> &Function::getParams() const {
        return params;
    }

    const std::shared_ptr<Statement> &Function::getBody() const {
        return body;
    }

    size_t Function::getLParenIndex() const {
        return lParenIndex;
    }

    size_t Function::getRParenIndex() const {
        return rParenIndex;
    }
}