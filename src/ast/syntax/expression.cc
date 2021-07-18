#include <utility>
#include <rang/rang.h>
#include <ast/syntax/expression.h>

namespace kisyshot::ast::syntax {
    void BinaryExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(left, false);
        syntaxWalker(right, true);
    }

    void BinaryExpression::writeCurrentInfo(std::ostream &s) {
        Expression::writeCurrentInfo(s);
    }

    void BinaryExpression::analyseType() {

    }

    SyntaxType BinaryExpression::getType() {
        return SyntaxType::BinaryExpression;
    }

    bool BinaryExpression::hasChild() {
        return true;
    }

    std::size_t BinaryExpression::start() {
        return left->start();
    }

    std::size_t BinaryExpression::end() {
        if (right != nullptr)
            return right->end();
        return opIndex;
    }

    std::string BinaryExpression::toString() {
        return left->toString() + " " + getTokenSpell(operatorType) + " " + right->toString();
    }

    void UnaryExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(right, true);
    }

    void UnaryExpression::writeCurrentInfo(std::ostream &s) {
        Expression::writeCurrentInfo(s);
    }

    void UnaryExpression::analyseType() {

    }

    SyntaxType UnaryExpression::getType() {
        return SyntaxType::UnaryExpression;
    }

    bool UnaryExpression::hasChild() {
        return true;
    }

    std::size_t UnaryExpression::start() {
        return opIndex;
    }

    std::size_t UnaryExpression::end() {
        if (right != nullptr)
            return right->end();
        return opIndex;
    }

    std::string UnaryExpression::toString() {
        return getTokenSpell(operatorType) + right->toString();
    }

    void IdentifierExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(name, true);
    }

    void IdentifierExpression::writeCurrentInfo(std::ostream &s) {
        Expression::writeCurrentInfo(s);
    }

    SyntaxType IdentifierExpression::getType() {
        return SyntaxType::IdentifierExpression;
    }

    bool IdentifierExpression::hasChild() {
        return true;
    }

    void IdentifierExpression::analyseType() {

    }

    std::size_t IdentifierExpression::start() {
        return name->start();
    }

    std::size_t IdentifierExpression::end() {
        return name->end();
    }

    std::string IdentifierExpression::toString() {
        return name->toString();
    }

    void Expression::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::gray << getType()
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::green << "'" << toString() << "' "
              << rang::fg::reset << std::endl;
        } else {
            s << " " << getType()
              << "<" << this << "> "
              << "'" << toString() << "' " << std::endl;
        }
    }

    void ParenthesesExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(innerExpression, true);
    }

    void ParenthesesExpression::writeCurrentInfo(std::ostream &ostream) {
        Expression::writeCurrentInfo(ostream);
    }

    SyntaxType ParenthesesExpression::getType() {
        return SyntaxType::ParenthesesExpression;
    }

    std::size_t ParenthesesExpression::start() {
        return leftParenIndex;
    }

    std::size_t ParenthesesExpression::end() {
        if (rightParenIndex != invalidTokenIndex)
            return rightParenIndex;
        if (innerExpression != nullptr)
            return innerExpression->end();
        return leftParenIndex;
    }

    bool ParenthesesExpression::hasChild() {
        return true;
    }

    void ParenthesesExpression::analyseType() {

    }

    std::string ParenthesesExpression::toString() {
        return "(" + innerExpression->toString() + ")";
    }

    void IndexExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(indexedExpr, false);
        syntaxWalker(indexerExpr, true);
    }

    void IndexExpression::writeCurrentInfo(std::ostream &ostream) {
        Expression::writeCurrentInfo(ostream);
    }

    SyntaxType IndexExpression::getType() {
        return SyntaxType::IndexExpression;
    }

    std::size_t IndexExpression::start() {
        if (indexedExpr != nullptr)
            return indexedExpr->start();
        return lSquareIndex;
    }

    std::size_t IndexExpression::end() {
        if (rSquareIndex != invalidTokenIndex)
            return rSquareIndex;
        if (indexerExpr != nullptr)
            return indexerExpr->end();
        return lSquareIndex;
    }

    bool IndexExpression::hasChild() {
        return true;
    }

    void IndexExpression::analyseType() {

    }

    std::string IndexExpression::toString() {
        return indexedExpr->toString() + "[" + indexerExpr->toString() + "]";
    }

    void CallExpression::add(const std::shared_ptr<Expression> &child) {
        arguments.push_back(child);
    }

    void CallExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(name, arguments.empty());
        for (std::size_t i = 0; i < arguments.size(); ++i) {
            syntaxWalker(arguments[i], i == arguments.size() - 1);
        }
    }

    void CallExpression::writeCurrentInfo(std::ostream &ostream) {
        Expression::writeCurrentInfo(ostream);
    }

    SyntaxType CallExpression::getType() {
        return SyntaxType::CallExpression;
    }

    std::size_t CallExpression::start() {
        return name->start();
    }

    std::size_t CallExpression::end() {
        if (rParenIndex != invalidTokenIndex)
            return rParenIndex;
        if (!arguments.empty()) {
            for (std::size_t i = 0; i <= arguments.size(); i++) {
                if (arguments[arguments.size() - i] != nullptr)
                    return arguments[arguments.size() - i]->end();
            }
        }
        return lParenIndex;
    }

    bool CallExpression::hasChild() {
        return true;
    }

    void CallExpression::analyseType() {

    }

    std::string CallExpression::toString() {
        std::string result = name->toString() + "(";
        for (std::size_t i = 0; i < arguments.size(); ++i) {
            result += arguments[i]->toString();
            if (i != arguments.size() - 1)
                result += ", ";
        }
        return result + ")";
    }

    void
    NumericLiteralExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
    }

    void NumericLiteralExpression::writeCurrentInfo(std::ostream &ostream) {
        Expression::writeCurrentInfo(ostream);
    }

    SyntaxType NumericLiteralExpression::getType() {
        return SyntaxType::NumericLiteralExpression;
    }

    std::size_t NumericLiteralExpression::start() {
        return tokenIndex;
    }

    std::size_t NumericLiteralExpression::end() {
        return tokenIndex;
    }

    bool NumericLiteralExpression::hasChild() {
        return false;
    }

    void NumericLiteralExpression::analyseType() {

    }

    std::string NumericLiteralExpression::toString() {
        return (std::string) rawCode;
    }

    void ArrayInitializeExpression::analyseType() {

    }

    std::string ArrayInitializeExpression::toString() {
        std::string result = "{ ";
        forEachChild([&result](const std::weak_ptr<SyntaxNode> &node, bool last) -> void {

            result += std::dynamic_pointer_cast<Expression>(node.lock())->toString();
            if (!last)
                result += ", ";
        });
        return result + " }";
    }

    void
    ArrayInitializeExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {

        for (size_t id = 0; id < array.size(); id++) {
            syntaxWalker(array[id], id == array.size() - 1);
        }
    }

    void ArrayInitializeExpression::writeCurrentInfo(std::ostream &ostream) {
        Expression::writeCurrentInfo(ostream);
    }

    std::size_t ArrayInitializeExpression::start() {
        return lBraceIndex;
    }

    std::size_t ArrayInitializeExpression::end() {
        if (rBraceIndex != SyntaxNode::invalidTokenIndex)
            return rBraceIndex;

        return array.back()->end();
    }

    SyntaxType ArrayInitializeExpression::getType() {
        return SyntaxType::ArrayInitializeExpression;
    }

    bool ArrayInitializeExpression::hasChild() {
        return !array.empty();
    }

    void ArrayInitializeExpression::add(const std::shared_ptr<Expression> &child) {
        array.push_back(child);
    }
}