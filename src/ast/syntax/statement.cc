#include <rang/rang.h>
#include <ast/syntax/statement.h>

namespace kisyshot::ast::syntax {
    IfStatement::IfStatement(std::size_t ifTokenIndex,
                             const std::shared_ptr<Expression> &condition,
                             const std::shared_ptr<Statement> &ifClause,
                             const std::shared_ptr<Statement> &elseClause) {
        _if = ifClause;
        _else = elseClause;
        _condition = condition;
        _ifTokenIndex = ifTokenIndex;
    }

    void IfStatement::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(_condition, false);
        if (_else == nullptr) {
            syntaxWalker(_if, true);
        } else {
            syntaxWalker(_if, false);
            syntaxWalker(_else, true);
        }
    }

    void IfStatement::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::magenta << "IfStatement "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::blue << "condition: "
              << rang::fg::green << "'" << _condition->toString() << "'"
              << rang::fg::reset << std::endl;
        } else {
            s << "IfStatement "
              << "<" << this << "> "
              << "condition: "
              << "'" << _condition->toString() << "'"
              << std::endl;
        }
    }

    SyntaxType IfStatement::getType() {
        return SyntaxType::IfStatement;
    }

    bool IfStatement::hasChild() {
        return true;
    }

    std::size_t IfStatement::start() {
        return _ifTokenIndex;
    }

    std::size_t IfStatement::end() {
        if (_else != nullptr)
            return _else->end();
        return _if->end();
    }

    void IfStatement::setLParenIndex(size_t lParenIndex) {
        _lParenIndex = lParenIndex;
    }

    void IfStatement::setRParenIndex(size_t rParenIndex) {
        _rParenIndex = rParenIndex;
    }

    void IfStatement::setElse(const std::shared_ptr<Statement> &elseClause) {
        _else = elseClause;
    }

    const std::shared_ptr<Statement> &IfStatement::getIf() const {
        return _if;
    }

    const std::shared_ptr<Statement> &IfStatement::getElse() const {
        return _else;
    }

    const std::shared_ptr<Expression> &IfStatement::getCondition() const {
        return _condition;
    }

    size_t IfStatement::getIfTokenIndex() const {
        return _ifTokenIndex;
    }

    size_t IfStatement::getLParenIndex() const {
        return _lParenIndex;
    }

    size_t IfStatement::getRParenIndex() const {
        return _rParenIndex;
    }

    void IfStatement::setElseTokenIndex(size_t index) {
        _elseTokenIndex = index;
    }

    size_t IfStatement::getElseTokenIndex() const {
        return _elseTokenIndex;
    }


    WhileStatement::WhileStatement(std::size_t whileTokenIndex,
                                   const std::shared_ptr<Expression> &condition,
                                   const std::shared_ptr<Statement> &body) {
        _whileTokenIndex = whileTokenIndex;
        _condition = condition;
        _body = body;
    }

    void WhileStatement::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(_condition, false);
        syntaxWalker(_body, true);
    }

    void WhileStatement::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::magenta << "WhileStatement "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::blue << "condition: "
              << rang::fg::green << "'" << _condition->toString() << "'"
              << rang::fg::reset << std::endl;
        } else {
            s << "WhileStatement "
              << "<" << this << "> "
              << "condition: "
              << "'" << _condition->toString() << "'"
              << std::endl;
        }
    }

    SyntaxType WhileStatement::getType() {
        return SyntaxType::WhileStatement;
    }

    bool WhileStatement::hasChild() {
        return true;
    }

    std::size_t WhileStatement::start() {
        return _whileTokenIndex;
    }

    std::size_t WhileStatement::end() {
        return _body->end();
    }

    void WhileStatement::setLParenIndex(size_t lParenIndex) {
        _lParenIndex = lParenIndex;
    }

    void WhileStatement::setRParenIndex(size_t rParenIndex) {
        _rParenIndex = rParenIndex;
    }

    size_t WhileStatement::getWhileTokenIndex() const {
        return _whileTokenIndex;
    }

    size_t WhileStatement::getLParenIndex() const {
        return _lParenIndex;
    }

    size_t WhileStatement::getRParenIndex() const {
        return _rParenIndex;
    }

    const std::shared_ptr<Expression> &WhileStatement::getCondition() const {
        return _condition;
    }

    const std::shared_ptr<Statement> &WhileStatement::getBody() const {
        return _body;
    }

    NopStatement::NopStatement(std::size_t semiTokenIndex) {
        _semiTokenIndex = semiTokenIndex;
    }

    void NopStatement::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
    }

    void NopStatement::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::magenta << "NopStatement "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::reset << std::endl;
        } else {
            s << "NopStatement "
              << "<" << this << "> "
              << std::endl;
        }
    }

    SyntaxType NopStatement::getType() {
        return SyntaxType::NopStatement;
    }

    bool NopStatement::hasChild() {
        return false;
    }

    std::size_t NopStatement::start() {
        return _semiTokenIndex;
    }

    std::size_t NopStatement::end() {
        return _semiTokenIndex;
    }

    BreakStatement::BreakStatement(std::size_t breakTokenIndex, std::size_t semiTokenIndex) {
        _breakTokenIndex = breakTokenIndex;
        _semiTokenIndex = semiTokenIndex;
    }

    void BreakStatement::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
    }

    void BreakStatement::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::magenta << "BreakStatement "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::reset << std::endl;
        } else {
            s << "BreakStatement "
              << "<" << this << "> "
              << std::endl;
        }
    }

    SyntaxType BreakStatement::getType() {
        return SyntaxType::BreakStatement;
    }

    bool BreakStatement::hasChild() {
        return false;
    }

    std::size_t BreakStatement::start() {
        return _breakTokenIndex;
    }

    std::size_t BreakStatement::end() {
        return _semiTokenIndex == invalidTokenIndex ? _breakTokenIndex : _semiTokenIndex;
    }

    ContinueStatement::ContinueStatement(std::size_t continueTokenIndex, std::size_t semiTokenIndex) {
        _continueTokenIndex = continueTokenIndex;
        _semiTokenIndex = semiTokenIndex;
    }

    void ContinueStatement::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {

    }

    void ContinueStatement::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::magenta << "ContinueStatement "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::reset << std::endl;
        } else {
            s << "ContinueStatement "
              << "<" << this << "> "
              << std::endl;
        }
    }

    SyntaxType ContinueStatement::getType() {
        return SyntaxType::ContinueStatement;
    }

    bool ContinueStatement::hasChild() {
        return false;
    }

    std::size_t ContinueStatement::start() {
        return _continueTokenIndex;
    }

    std::size_t ContinueStatement::end() {
        return _semiTokenIndex == invalidTokenIndex ? _continueTokenIndex : _semiTokenIndex;
    }

    ReturnStatement::ReturnStatement(std::size_t returnTokenIndex, std::size_t semiTokenIndex) {
        _returnTokenIndex = returnTokenIndex;
        _semiTokenIndex = semiTokenIndex;
        _value = nullptr;
    }

    void ReturnStatement::setValue(const std::shared_ptr<Expression> &value) {
        _value = value;
    }

    void ReturnStatement::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        if (_value != nullptr)
            syntaxWalker(_value, true);
    }

    void ReturnStatement::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::magenta << "ReturnStatement "
              << rang::fg::yellow << "<" << this << "> ";
            if (_value != nullptr)
                s << rang::fg::green << "value: '" << _value->toString() << "'";
            s << rang::fg::reset << std::endl;
        } else {
            s << "ReturnStatement "
              << "<" << this << "> ";
            if (_value != nullptr)
                s << rang::fg::green << "value: '" << _value->toString() << "'";
            s << std::endl;
        }
    }

    SyntaxType ReturnStatement::getType() {
        return SyntaxType::ReturnStatement;
    }

    bool ReturnStatement::hasChild() {
        return _value != nullptr;
    }

    std::size_t ReturnStatement::start() {
        return _returnTokenIndex;
    }

    std::size_t ReturnStatement::end() {
        if (_semiTokenIndex != invalidTokenIndex)
            return _semiTokenIndex;
        if (_value != nullptr)
            return _value->end();
        return _returnTokenIndex;
    }

    void ReturnStatement::setSemiTokenIndex(size_t semiTokenIndex) {
        _semiTokenIndex = semiTokenIndex;
    }

    void BlockStatement::add(const std::shared_ptr<Statement> &child) {
        _children.push_back(child);
    }

    void BlockStatement::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::magenta << "BlockStatement "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::reset << std::endl;
        } else {
            s << "BlockStatement "
              << "<" << this << "> " << std::endl;
        }
    }

    void BlockStatement::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        for (size_t id = 0; id < _children.size(); id++) {
            syntaxWalker(_children[id], id == _children.size() - 1);
        }
    }

    SyntaxType BlockStatement::getType() {
        return SyntaxType::BlockStatement;
    }

    bool BlockStatement::hasChild() {
        return !_children.empty();
    }

    std::size_t BlockStatement::start() {
        return _lBraceTokenIndex;
    }

    std::size_t BlockStatement::end() {
        if (_rBraceTokenIndex != invalidTokenIndex)
            return _rBraceTokenIndex;
        if (_children.empty())
            return _lBraceTokenIndex;
        return _children.back()->end();
    }

    BlockStatement::BlockStatement(std::size_t lBraceTokenIndex) {
        _lBraceTokenIndex = lBraceTokenIndex;
        _rBraceTokenIndex = invalidTokenIndex;
    }

    void BlockStatement::setRBraceTokenIndex(std::size_t rBraceTokenIndex) {
        _rBraceTokenIndex = rBraceTokenIndex;
    }

    ExpressionStatement::ExpressionStatement(const std::shared_ptr<Expression> &expression) {
        _expression = expression;
        _semiTokenIndex = invalidTokenIndex;
    }

    void ExpressionStatement::setSemiTokenIndex(std::size_t semiTokenIndex) {
        _semiTokenIndex = semiTokenIndex;
    }

    void ExpressionStatement::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(_expression, true);
    }

    void ExpressionStatement::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::magenta << "ExpressionStatement "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::blue << "expr: "
              << rang::fg::green << "'" << _expression->toString() << "'"
              << rang::fg::reset << std::endl;
        } else {
            s << "ExpressionStatement "
              << "<" << this << "> "
              << "expr: "
              << "'" << _expression->toString() << "'"
              << std::endl;
        }
    }

    SyntaxType ExpressionStatement::getType() {
        return SyntaxType::ExpressionStatement;
    }

    bool ExpressionStatement::hasChild() {
        return true;
    }

    std::size_t ExpressionStatement::start() {
        return _expression->start();
    }

    std::size_t ExpressionStatement::end() {
        if (_semiTokenIndex == invalidTokenIndex)
            return _expression->end();
        return _semiTokenIndex;
    }
}