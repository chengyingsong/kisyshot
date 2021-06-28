#pragma once

#include <ast/token.h>
#include "identifier.h"
#include "syntax_node.h"

namespace kisyshot::ast::syntax{

    class Expression:public SyntaxNode{
    public:
        virtual void analyseType() = 0;
        virtual std::string toString() = 0;
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override = 0;
        void writeCurrentInfo(std::ostream& ostream) override = 0;
        std::size_t start() override = 0;
        std::size_t end() override = 0;
        SyntaxType getType() override = 0;
        bool hasChild() override = 0;
    };

    class BinaryExpression: public Expression {
    public:
        BinaryExpression(const std::shared_ptr<Expression>& left,
                         TokenType operatorType,
                         std::size_t opIndex,
                         const std::shared_ptr<Expression>& right);
        TokenType getOperatorType();
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>,bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        std::size_t start() override;
        std::size_t end() override;
        bool hasChild() override ;
        void analyseType() override;
        std::string toString() override;

        const std::shared_ptr<Expression> &getLeft() const;

        size_t getOpIndex() const;

        const std::shared_ptr<Expression> &getRight() const;

    private:
        std::shared_ptr<Expression> _left;
        TokenType _operatorType;
        std::size_t _opIndex;
        std::shared_ptr<Expression> _right;
    };

    class UnaryExpression: public Expression{
    public:
        UnaryExpression(TokenType operatorType, std::size_t opIndex, const std::shared_ptr<Expression>& right);
        TokenType getOperatorType();
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream &ostream) override;
        SyntaxType getType() override;
        std::size_t start() override;
        std::size_t end() override;
        bool hasChild() override;
        void analyseType() override;
        std::string toString() override;

        size_t getOpIndex() const;

        const std::shared_ptr<Expression> &getRight() const;

    private:
        TokenType _operatorType;
        std::size_t _opIndex;
        std::shared_ptr<Expression> _right;
    };

    class IdentifierExpression: public Expression{
    public:
        explicit IdentifierExpression(const std::shared_ptr<Identifier>& name);
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        std::size_t start() override ;
        std::size_t end() override ;
        bool hasChild() override ;
        void analyseType() override ;
        std::string toString() override ;
        std::shared_ptr<Identifier> getIdentifier();

    private:
        std::shared_ptr<Identifier> _name;
    };

    class ParenthesesExpression: public Expression{
    public:
        ParenthesesExpression(std::size_t lParenIndex,std::size_t rParenIndex,const std::shared_ptr<Expression> &innerExpr);
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        std::size_t start() override ;
        std::size_t end() override ;
        bool hasChild() override ;
        void analyseType() override ;
        std::string toString() override ;

    private:
        std::size_t _leftParenIndex;
        std::size_t _rightParenIndex;
        std::shared_ptr<Expression> _innerExpression;
    };

    class IndexExpression:public Expression{
    public:
        IndexExpression(const std::shared_ptr<Expression> &indexedExpr,const std::shared_ptr<Expression> &index,
                std::size_t lSquareIndex,std::size_t rSquareIndex = invalidTokenIndex);
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        std::size_t start() override ;
        std::size_t end() override ;
        bool hasChild() override ;
        void analyseType() override ;
        std::string toString() override ;

        const std::shared_ptr<Expression> &getIndexedExpr() const;

        const std::shared_ptr<Expression> &getIndex() const;

        size_t getLSquareIndex() const;

        size_t getRSquareIndex() const;

    private:
        std::shared_ptr<Expression> _indexedExpr;
        std::shared_ptr<Expression> _index;
        std::size_t _lSquareIndex;
        std::size_t _rSquareIndex;
    };

    class CallExpression: public Expression, public ISyntaxList<Expression> {
    public:
        explicit CallExpression(std::size_t lParenIndex, const std::shared_ptr<Identifier> &methodName,
                                std::size_t rParenIndex = invalidTokenIndex);
        void setRParenIndex(std::size_t rParenIndex);
        void add(const std::shared_ptr<Expression> &child) override;
        void forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream &ostream) override;
        SyntaxType getType() override;
        std::size_t start() override;
        std::size_t end() override;
        bool hasChild() override;
        void analyseType() override;
        std::string toString() override;

        const std::shared_ptr<Identifier> &getName() const;

        const std::vector<std::shared_ptr<Expression>> &getArguments() const;

        size_t getLParenIndex() const;

        size_t getRParenIndex() const;

    private:
        std::shared_ptr<Identifier> _name;
        std::vector<std::shared_ptr<Expression>> _arguments;
        std::size_t _lParenIndex;
        std::size_t _rParenIndex;
    };



    class NumericLiteralExpression:public Expression{
    public:
        NumericLiteralExpression(const std::string_view &rawCode,std::size_t numericIndex);
        void forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream &ostream) override;
        SyntaxType getType() override;
        std::size_t start() override;
        std::size_t end() override;
        bool hasChild() override;
        void analyseType() override;
        std::string toString() override;

    private:
        std::size_t _numericIndex;
        std::string_view _rawCode;
    };
    class ArrayInitializeExpression: public Expression, public ISyntaxList<Expression>{
    public:
        explicit ArrayInitializeExpression(std::size_t lParenIndex);
        void analyseType() override;
        void add(const std::shared_ptr<Expression> &child) override;
        std::string toString() override;
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        std::size_t start() override;
        std::size_t end() override;
        SyntaxType getType() override;
        bool hasChild() override;

    private:
        std::size_t _lParenIndex, _rParenIndex;
        std::vector<std::shared_ptr<Expression>> _array;
    };
}
