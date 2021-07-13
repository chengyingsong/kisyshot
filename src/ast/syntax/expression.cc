#include <utility>
#include <rang/rang.h>
#include <ast/syntax/expression.h>

namespace kisyshot::ast::syntax {
    void BinaryExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(left, false);
        syntaxWalker(right, true);
    }

    BinaryExpression::BinaryExpression(const std::shared_ptr<Expression> &left,
                                       TokenType operatorType,
                                       std::size_t opIndex,
                                       const std::shared_ptr<Expression> &right) {
        this->left = left;
        this->right = right;
        this->operatorType = operatorType;
        this->opIndex = opIndex;
    }

    TokenType BinaryExpression::getOperatorType() {
        return operatorType;
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

    const std::shared_ptr<Expression> &BinaryExpression::getLeft() const {
        return left;
    }

    size_t BinaryExpression::getOpIndex() const {
        return opIndex;
    }

    const std::shared_ptr<Expression> &BinaryExpression::getRight() const {
        return right;
    }


    void UnaryExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(right, true);
    }

    UnaryExpression::UnaryExpression(TokenType operatorType,
                                     std::size_t opIndex,
                                     const std::shared_ptr<Expression> &right) {
        this->right = right;
        this->opIndex = opIndex;
        this->operatorType = operatorType;
    }

    TokenType UnaryExpression::getOperatorType() {
        return operatorType;
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

    size_t UnaryExpression::getOpIndex() const {
        return opIndex;
    }

    const std::shared_ptr<Expression> &UnaryExpression::getRight() const {
        return right;
    }

    IdentifierExpression::IdentifierExpression(const std::shared_ptr<Identifier> &name) {
        this->name = name;
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

    std::shared_ptr<Identifier> IdentifierExpression::getIdentifier() {
        return name;
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

    ParenthesesExpression::ParenthesesExpression(std::size_t lParenIndex, std::size_t rParenIndex,
                                                 const std::shared_ptr<Expression> &innerExpr) {
        leftParenIndex = lParenIndex;
        rightParenIndex = rParenIndex;
        innerExpression = innerExpr;
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

    IndexExpression::IndexExpression(const std::shared_ptr<Expression> &indexedExpr,
                                     const std::shared_ptr<Expression> &index, std::size_t lSquareIndex,
                                     std::size_t rSquareIndex) {
        this->indexedExpr = indexedExpr;
        this->indexerExpr = index;
        this->lSquareIndex = lSquareIndex;
        this->rSquareIndex = rSquareIndex;
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

    const std::shared_ptr<Expression> &IndexExpression::getIndexedExpr() const {
        return indexedExpr;
    }

    const std::shared_ptr<Expression> &IndexExpression::getIndex() const {
        return indexerExpr;
    }

    size_t IndexExpression::getLSquareIndex() const {
        return lSquareIndex;
    }

    size_t IndexExpression::getRSquareIndex() const {
        return rSquareIndex;
    }

    CallExpression::CallExpression(std::size_t lParenIndex, const std::shared_ptr<Identifier> &functionName,
                                   std::size_t rParenIndex) {
        lParenIndex = lParenIndex;
        name = functionName;
        rParenIndex = rParenIndex;
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

    void CallExpression::setRParenIndex(std::size_t rParenIndex) {
        rParenIndex = rParenIndex;
    }

    const std::shared_ptr<Identifier> &CallExpression::getName() const {
        return name;
    }

    const std::vector<std::shared_ptr<Expression>> &CallExpression::getArguments() const {
        return arguments;
    }

    size_t CallExpression::getLParenIndex() const {
        return lParenIndex;
    }

    size_t CallExpression::getRParenIndex() const {
        return rParenIndex;
    }

    NumericLiteralExpression::NumericLiteralExpression(const std::string_view &rawCode, std::size_t numericIndex) {
        this->tokenIndex = numericIndex;
        this->rawCode = rawCode;
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

    ArrayInitializeExpression::ArrayInitializeExpression(std::size_t lParenIndex) {
        lParenIndex = lParenIndex;
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
        return lParenIndex;
    }

    std::size_t ArrayInitializeExpression::end() {
        if (rParenIndex != SyntaxNode::invalidTokenIndex)
            return rParenIndex;

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