#include <utility>
#include <rang/rang.h>
#include <ast/syntax/expression.h>

namespace kisyshot::ast::syntax {
    void BinaryExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(_left, false);
        syntaxWalker(_right, true);
    }

    BinaryExpression::BinaryExpression(const std::shared_ptr<Expression> &left,
                                       TokenType operatorType,
                                       std::size_t opIndex,
                                       const std::shared_ptr<Expression> &right) {
        _left = left;
        _right = right;
        _operatorType = operatorType;
        _opIndex = opIndex;
    }

    TokenType BinaryExpression::getOperatorType() {
        return _operatorType;
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
        return _left->start();
    }

    std::size_t BinaryExpression::end() {
        if (_right != nullptr)
            return _right->end();
        return _opIndex;
    }

    std::string BinaryExpression::toString() {
        return _left->toString() + " " + getTokenSpell(_operatorType) + " " + _right->toString();
    }

    const std::shared_ptr<Expression> &BinaryExpression::getLeft() const {
        return _left;
    }

    size_t BinaryExpression::getOpIndex() const {
        return _opIndex;
    }

    const std::shared_ptr<Expression> &BinaryExpression::getRight() const {
        return _right;
    }


    void UnaryExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(_right, true);
    }

    UnaryExpression::UnaryExpression(TokenType operatorType,
                                     std::size_t opIndex,
                                     const std::shared_ptr<Expression> &right) {
        _right = right;
        _opIndex = opIndex;
        _operatorType = operatorType;
    }

    TokenType UnaryExpression::getOperatorType() {
        return _operatorType;
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
        return _opIndex;
    }

    std::size_t UnaryExpression::end() {
        if (_right != nullptr)
            return _right->end();
        return _opIndex;
    }

    std::string UnaryExpression::toString() {
        return getTokenSpell(_operatorType) + _right->toString();
    }

    size_t UnaryExpression::getOpIndex() const {
        return _opIndex;
    }

    const std::shared_ptr<Expression> &UnaryExpression::getRight() const {
        return _right;
    }

    IdentifierExpression::IdentifierExpression(const std::shared_ptr<Identifier> &name) {
        _name = name;
    }

    void IdentifierExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(_name, true);
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
        return _name->start();
    }

    std::size_t IdentifierExpression::end() {
        return _name->end();
    }

    std::string IdentifierExpression::toString() {
        return _name->toString();
    }

    std::shared_ptr<Identifier> IdentifierExpression::getIdentifier() {
        return _name;
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
        _leftParenIndex = lParenIndex;
        _rightParenIndex = rParenIndex;
        _innerExpression = innerExpr;
    }

    void ParenthesesExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(_innerExpression, true);
    }

    void ParenthesesExpression::writeCurrentInfo(std::ostream &ostream) {
        Expression::writeCurrentInfo(ostream);
    }

    SyntaxType ParenthesesExpression::getType() {
        return SyntaxType::ParenthesesExpression;
    }

    std::size_t ParenthesesExpression::start() {
        return _leftParenIndex;
    }

    std::size_t ParenthesesExpression::end() {
        if (_rightParenIndex != invalidTokenIndex)
            return _rightParenIndex;
        if (_innerExpression != nullptr)
            return _innerExpression->end();
        return _leftParenIndex;
    }

    bool ParenthesesExpression::hasChild() {
        return true;
    }

    void ParenthesesExpression::analyseType() {

    }

    std::string ParenthesesExpression::toString() {
        return "(" + _innerExpression->toString() + ")";
    }

    IndexExpression::IndexExpression(const std::shared_ptr<Expression> &indexedExpr,
                                     const std::shared_ptr<Expression> &index, std::size_t lSquareIndex,
                                     std::size_t rSquareIndex) {
        _indexedExpr = indexedExpr;
        _index = index;
        _lSquareIndex = lSquareIndex;
        _rSquareIndex = rSquareIndex;
    }

    void IndexExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(_indexedExpr, false);
        syntaxWalker(_index, true);
    }

    void IndexExpression::writeCurrentInfo(std::ostream &ostream) {
        Expression::writeCurrentInfo(ostream);
    }

    SyntaxType IndexExpression::getType() {
        return SyntaxType::IndexExpression;
    }

    std::size_t IndexExpression::start() {
        if (_indexedExpr != nullptr)
            return _indexedExpr->start();
        return _lSquareIndex;
    }

    std::size_t IndexExpression::end() {
        if (_rSquareIndex != invalidTokenIndex)
            return _rSquareIndex;
        if (_index != nullptr)
            return _index->end();
        return _lSquareIndex;
    }

    bool IndexExpression::hasChild() {
        return true;
    }

    void IndexExpression::analyseType() {

    }

    std::string IndexExpression::toString() {
        return _indexedExpr->toString() + "[" + _index->toString() + "]";
    }

    const std::shared_ptr<Expression> &IndexExpression::getIndexedExpr() const {
        return _indexedExpr;
    }

    const std::shared_ptr<Expression> &IndexExpression::getIndex() const {
        return _index;
    }

    size_t IndexExpression::getLSquareIndex() const {
        return _lSquareIndex;
    }

    size_t IndexExpression::getRSquareIndex() const {
        return _rSquareIndex;
    }

    CallExpression::CallExpression(std::size_t lParenIndex, const std::shared_ptr<Identifier> &functionName,
                                   std::size_t rParenIndex) {
        _lParenIndex = lParenIndex;
        _name = functionName;
        _rParenIndex = rParenIndex;
    }

    void CallExpression::add(const std::shared_ptr<Expression> &child) {
        _arguments.push_back(child);
    }

    void CallExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(_name, _arguments.empty());
        for (std::size_t i = 0; i < _arguments.size(); ++i) {
            syntaxWalker(_arguments[i], i == _arguments.size() - 1);
        }
    }

    void CallExpression::writeCurrentInfo(std::ostream &ostream) {
        Expression::writeCurrentInfo(ostream);
    }

    SyntaxType CallExpression::getType() {
        return SyntaxType::CallExpression;
    }

    std::size_t CallExpression::start() {
        return _name->start();
    }

    std::size_t CallExpression::end() {
        if (_rParenIndex != invalidTokenIndex)
            return _rParenIndex;
        if (!_arguments.empty()) {
            for (std::size_t i = 0; i <= _arguments.size(); i++) {
                if (_arguments[_arguments.size() - i] != nullptr)
                    return _arguments[_arguments.size() - i]->end();
            }
        }
        return _lParenIndex;
    }

    bool CallExpression::hasChild() {
        return true;
    }

    void CallExpression::analyseType() {

    }

    std::string CallExpression::toString() {
        std::string result = _name->toString() + "(";
        for (std::size_t i = 0; i < _arguments.size(); ++i) {
            result += _arguments[i]->toString();
            if (i != _arguments.size() - 1)
                result += ", ";
        }
        return result + ")";
    }

    void CallExpression::setRParenIndex(std::size_t rParenIndex) {
        _rParenIndex = rParenIndex;
    }

    const std::shared_ptr<Identifier> &CallExpression::getName() const {
        return _name;
    }

    const std::vector<std::shared_ptr<Expression>> &CallExpression::getArguments() const {
        return _arguments;
    }

    size_t CallExpression::getLParenIndex() const {
        return _lParenIndex;
    }

    size_t CallExpression::getRParenIndex() const {
        return _rParenIndex;
    }

    NumericLiteralExpression::NumericLiteralExpression(const std::string_view &rawCode, std::size_t numericIndex) {
        _numericIndex = numericIndex;
        _rawCode = rawCode;
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
        return _numericIndex;
    }

    std::size_t NumericLiteralExpression::end() {
        return _numericIndex;
    }

    bool NumericLiteralExpression::hasChild() {
        return false;
    }

    void NumericLiteralExpression::analyseType() {

    }

    std::string NumericLiteralExpression::toString() {
        return (std::string) _rawCode;
    }

    ArrayInitializeExpression::ArrayInitializeExpression(std::size_t lParenIndex) {
        _lParenIndex = lParenIndex;
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

        for (size_t id = 0; id < _array.size(); id++) {
            syntaxWalker(_array[id], id == _array.size() - 1);
        }
    }

    void ArrayInitializeExpression::writeCurrentInfo(std::ostream &ostream) {
        Expression::writeCurrentInfo(ostream);
    }

    std::size_t ArrayInitializeExpression::start() {
        return _lParenIndex;
    }

    std::size_t ArrayInitializeExpression::end() {
        if (_rParenIndex != SyntaxNode::invalidTokenIndex)
            return _rParenIndex;

        return _array.back()->end();
    }

    SyntaxType ArrayInitializeExpression::getType() {
        return SyntaxType::ArrayInitializeExpression;
    }

    bool ArrayInitializeExpression::hasChild() {
        return !_array.empty();
    }

    void ArrayInitializeExpression::add(const std::shared_ptr<Expression> &child) {
        _array.push_back(child);
    }
}