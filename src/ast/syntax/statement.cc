#include <rang/rang.h>
#include <ast/syntax/statement.h>
#include<iostream>

namespace kisyshot::ast::syntax {
    void IfStatement::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(condition, false);
        if (elseClause == nullptr) {
            syntaxWalker(ifClause, true);
        } else {
            syntaxWalker(ifClause, false);
            syntaxWalker(elseClause, true);
        }
    }

    void IfStatement::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::magenta << "IfStatement "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::blue << "condition: "
              << rang::fg::green << "'" << condition->toString() << "'"
              << rang::fg::reset << std::endl;
        } else {
            s << "IfStatement "
              << "<" << this << "> "
              << "condition: "
              << "'" << condition->toString() << "'"
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
        return ifTokenIndex;
    }

    std::size_t IfStatement::end() {
        if (elseClause != nullptr)
            return elseClause->end();
        return ifClause->end();
    }

    void IfStatement::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {
        /*
         * t1 = condition
         * if t1==0 GOTO falseLabel
         * trueStatement
         * GOTO endLabel
         * falseLabel:
         * falseStatement
         * endLabel:
         *
         * t1 = condition
         * if t1==0 GOTO falseLabel
         * trueStatement
         * GOTO endLabel
         * falseLabel:
         * endLabel
         * */
        //TODO: Statement的继承属性直接写在前面
        Var* t1 = condition->getVar(gen);
        if(elseClause != nullptr) {
            std::string falseLabel = gen.newLabel();
            std::string ifEndLabel = gen.newLabel();
            gen.genIFZ(t1, falseLabel);
            ifClause->inTheWhile = inTheWhile;
            ifClause->beginLabel = beginLabel;
            ifClause->endLabel = endLabel;
            ifClause->endFuncLabel = endFuncLabel;
            ifClause->genCode(gen, nullptr);
            gen.genGOTO(ifEndLabel);
            gen.genLabel(falseLabel);
            elseClause->inTheWhile = inTheWhile;
            elseClause->beginLabel = beginLabel;
            elseClause->endLabel = endLabel;
            elseClause->endFuncLabel = endFuncLabel;
            elseClause->genCode(gen, nullptr);
            gen.genLabel(ifEndLabel);
        } else{
            std::string falseLabel = gen.newLabel();
            std::string ifEndLabel = gen.newLabel();
            gen.genIFZ(t1,falseLabel);
            ifClause->inTheWhile = inTheWhile;
            ifClause->beginLabel = beginLabel;
            ifClause->endLabel = endLabel;
            ifClause->endFuncLabel = endFuncLabel;
            ifClause->genCode(gen, nullptr);
            gen.genGOTO(ifEndLabel);
            gen.genLabel(falseLabel);
            gen.genLabel(ifEndLabel);
        }
    }

    void WhileStatement::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(condition, false);
        syntaxWalker(body, true);
    }

    void WhileStatement::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::magenta << "WhileStatement "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::blue << "condition: "
              << rang::fg::green << "'" << condition->toString() << "'"
              << rang::fg::reset << std::endl;
        } else {
            s << "WhileStatement "
              << "<" << this << "> "
              << "condition: "
              << "'" << condition->toString() << "'"
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
        return whileTokenIndex;
    }

    std::size_t WhileStatement::end() {
        return body->end();
    }

    void WhileStatement::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {
        /*
         * BeginLabel:
         *  t = condition
         *  if t == 0 GOTO endLabel
         *  bodyStatement
         *  GOTO BeginLabel
         * endLabel:
         *
         * */
        std::string beginLabel =  gen.newLabel();
        gen.genLabel(beginLabel);
        Var* t = condition->getVar(gen);
        std::string endLabel = gen.newLabel();
        gen.genIFZ(t,endLabel);
        body->inTheWhile = true;
        body->beginLabel = beginLabel;
        body->endLabel = endLabel;
        body->endFuncLabel = endFuncLabel;
        body->genCode(gen, nullptr);
        gen.genGOTO(beginLabel);
        gen.genLabel(endLabel);

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
        return tokenIndex;
    }

    std::size_t NopStatement::end() {
        return tokenIndex;
    }

    void NopStatement::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {}

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
        return breakTokenIndex;
    }

    std::size_t BreakStatement::end() {
        return semiTokenIndex == invalidTokenIndex ? breakTokenIndex : semiTokenIndex;
    }

    void BreakStatement::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {
        gen.genGOTO(endLabel); //跳出循环
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
        return continueTokenIndex;
    }

    std::size_t ContinueStatement::end() {
        return semiTokenIndex == invalidTokenIndex ? continueTokenIndex : semiTokenIndex;
    }

    void ContinueStatement::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {
        gen.genGOTO(beginLabel); //跳转到下一轮循环开始
    }

    void ReturnStatement::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        if (value != nullptr)
            syntaxWalker(value, true);
    }

    void ReturnStatement::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::magenta << "ReturnStatement "
              << rang::fg::yellow << "<" << this << "> ";
            if (value != nullptr)
                s << rang::fg::green << "value: '" << value->toString() << "'";
            s << rang::fg::reset << std::endl;
        } else {
            s << "ReturnStatement "
              << "<" << this << "> ";
            if (value != nullptr)
                s << rang::fg::green << "value: '" << value->toString() << "'";
            s << std::endl;
        }
    }

    SyntaxType ReturnStatement::getType() {
        return SyntaxType::ReturnStatement;
    }

    bool ReturnStatement::hasChild() {
        return value != nullptr;
    }

    std::size_t ReturnStatement::start() {
        return returnTokenIndex;
    }

    std::size_t ReturnStatement::end() {
        if (semiTokenIndex != invalidTokenIndex)
            return semiTokenIndex;
        if (value != nullptr)
            return value->end();
        return returnTokenIndex;
    }

    void ReturnStatement::genCode(compiler::CodeGenerator &gen,ast::Var* temp) {
        Var *t = nullptr;
        if (value != nullptr) {
            //要求得返回表达式的值，构造返回语句
            t = value->getVar(gen);
        }
        gen.genReturn(t);
        gen.genGOTO(endFuncLabel);
    }

    void BlockStatement::add(const std::shared_ptr<Statement> &child) {
        children.push_back(child);
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
        for (size_t id = 0; id < children.size(); id++) {
            syntaxWalker(children[id], id == children.size() - 1);
        }
    }

    SyntaxType BlockStatement::getType() {
        return SyntaxType::BlockStatement;
    }

    bool BlockStatement::hasChild() {
        return !children.empty();
    }

    std::size_t BlockStatement::start() {
        return lBraceTokenIndex;
    }

    std::size_t BlockStatement::end() {
        if (rBraceTokenIndex != invalidTokenIndex)
            return rBraceTokenIndex;
        if (children.empty())
            return lBraceTokenIndex;
        return children.back()->end();
    }

    void BlockStatement::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {
        //std::cout << "   block statement" << std::endl;
        for (auto &child:children) {
            if(inTheWhile){
                child->inTheWhile = true;
                child->beginLabel = beginLabel;
                child->endLabel = endLabel;
            }
            child->endFuncLabel = endFuncLabel;
            child->genCode(gen, nullptr);

        }
    }

    void ExpressionStatement::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(expression, true);
    }

    void ExpressionStatement::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::magenta << "ExpressionStatement "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::blue << "expr: "
              << rang::fg::green << "'" << expression->toString() << "'"
              << rang::fg::reset << std::endl;
        } else {
            s << "ExpressionStatement "
              << "<" << this << "> "
              << "expr: "
              << "'" << expression->toString() << "'"
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
        return expression->start();
    }

    std::size_t ExpressionStatement::end() {
        if (semiTokenIndex == invalidTokenIndex)
            return expression->end();
        return semiTokenIndex;
    }

    void ExpressionStatement::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {
        expression->genCode(gen, nullptr);
    }

}