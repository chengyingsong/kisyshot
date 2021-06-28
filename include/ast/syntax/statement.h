#pragma once

#include "expression.h"
#include "type.h"

namespace kisyshot::ast::syntax{
    class Statement:public SyntaxNode{
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override = 0;
        void writeCurrentInfo(std::ostream& ostream) override = 0;
        SyntaxType getType() override = 0;
        bool hasChild() override = 0;
        std::size_t start() override = 0;
        std::size_t end() override = 0;
    };

    class IfStatement:public Statement{
    public:
        IfStatement(std::size_t ifTokenIndex,
                    const std::shared_ptr<Expression> &condition,
                    const std::shared_ptr<Statement>& ifClause,
                    const std::shared_ptr<Statement>& elseClause = nullptr);
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override ;
        void setElse(const std::shared_ptr<Statement> &elseClause);
        void setLParenIndex(size_t lParenIndex);
        void setRParenIndex(size_t rParenIndex);
        void setElseTokenIndex(size_t index);
        const std::shared_ptr<Statement> &getIf() const;
        const std::shared_ptr<Statement> &getElse() const;
        const std::shared_ptr<Expression> &getCondition() const;
        size_t getIfTokenIndex() const;
        size_t getElseTokenIndex() const;
        size_t getLParenIndex() const;
        size_t getRParenIndex() const;
    private:
        std::shared_ptr<Statement> _if;
        std::shared_ptr<Statement> _else;
        std::shared_ptr<Expression> _condition;
        std::size_t _ifTokenIndex;
        std::size_t _elseTokenIndex;
        std::size_t _lParenIndex;
        std::size_t _rParenIndex;
    };


    class WhileStatement:public Statement{
    public:
        WhileStatement(std::size_t whileTokenIndex,
                    const std::shared_ptr<Expression>& condition,
                    const std::shared_ptr<Statement>& body);
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override ;
        void setLParenIndex(size_t lParenIndex);
        void setRParenIndex(size_t rParenIndex);
        size_t getWhileTokenIndex() const;
        size_t getLParenIndex() const;
        size_t getRParenIndex() const;
        const std::shared_ptr<Expression> &getCondition() const;
        const std::shared_ptr<Statement> &getBody() const;
    private:
        std::size_t _whileTokenIndex;
        std::size_t _lParenIndex;
        std::size_t _rParenIndex;
        std::shared_ptr<Expression> _condition;
        std::shared_ptr<Statement> _body;
    };


    class NopStatement:public Statement{
    public:
        explicit NopStatement(std::size_t semiTokenIndex);
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override ;
    private:
        std::size_t _semiTokenIndex;
    };

    class BreakStatement:public Statement{
    public:
        explicit BreakStatement(std::size_t breakTokenIndex,std::size_t semiTokenIndex = invalidTokenIndex);
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override ;

    private:
        std::size_t _breakTokenIndex;
        std::size_t _semiTokenIndex;
    };

    class ContinueStatement:public Statement{
    public:
        explicit ContinueStatement(std::size_t continueTokenIndex,std::size_t semiTokenIndex = invalidTokenIndex);
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override ;

    private:
        std::size_t _continueTokenIndex;
        std::size_t _semiTokenIndex;
    };

    class ReturnStatement:public Statement{
    public:
        explicit ReturnStatement(std::size_t returnTokenIndex, std::size_t semiTokenIndex=invalidTokenIndex);
        void setValue(const std::shared_ptr<Expression> &value);
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override ;
        void setSemiTokenIndex(size_t semiTokenIndex);

    private:
        std::size_t _returnTokenIndex;
        std::size_t _semiTokenIndex;
        std::shared_ptr<Expression> _value;
    };

    class BlockStatement: public ISyntaxList<Statement>, public Statement{
    public:
        explicit BlockStatement(std::size_t lBraceTokenIndex);
        void setRBraceTokenIndex(std::size_t rBraceTokenIndex);
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override ;
        void add(const std::shared_ptr<Statement>& child) override ;

    private:
        std::vector<std::shared_ptr<Statement>> _children;
        std::size_t _lBraceTokenIndex;
        std::size_t _rBraceTokenIndex;
    };

    class ExpressionStatement: public Statement{
    public:
        explicit ExpressionStatement(const std::shared_ptr<Expression>& expression);
        void setSemiTokenIndex(std::size_t semiTokenIndex);
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override ;
    private:
        std::shared_ptr<Expression> _expression;
        std::size_t _semiTokenIndex;
    };
}