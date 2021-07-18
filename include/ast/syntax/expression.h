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
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>,bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        std::size_t start() override;
        std::size_t end() override;
        bool hasChild() override ;
        void analyseType() override;
        std::string toString() override;

        std::shared_ptr<Expression> left;
        TokenType operatorType;
        std::size_t opIndex;
        std::shared_ptr<Expression> right;
    };

    class UnaryExpression: public Expression{
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream &ostream) override;
        SyntaxType getType() override;
        std::size_t start() override;
        std::size_t end() override;
        bool hasChild() override;
        void analyseType() override;
        std::string toString() override;

        TokenType operatorType;
        std::size_t opIndex;
        std::shared_ptr<Expression> right;
    };

    class IdentifierExpression: public Expression{
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        std::size_t start() override ;
        std::size_t end() override ;
        bool hasChild() override ;
        void analyseType() override ;
        std::string toString() override ;

        std::shared_ptr<Identifier> name;
    };

    class ParenthesesExpression: public Expression{
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        std::size_t start() override ;
        std::size_t end() override ;
        bool hasChild() override ;
        void analyseType() override ;
        std::string toString() override ;

    
        std::size_t leftParenIndex;
        std::size_t rightParenIndex;
        std::shared_ptr<Expression> innerExpression;
    };

    class IndexExpression:public Expression{
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        std::size_t start() override ;
        std::size_t end() override ;
        bool hasChild() override ;
        void analyseType() override ;
        std::string toString() override ;
    
        std::shared_ptr<Expression> indexedExpr;
        std::shared_ptr<Expression> indexerExpr;
        std::size_t lSquareIndex;
        std::size_t rSquareIndex;
    };

    class CallExpression: public Expression, public ISyntaxList<Expression> {
    public:
        void add(const std::shared_ptr<Expression> &child) override;
        void forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream &ostream) override;
        SyntaxType getType() override;
        std::size_t start() override;
        std::size_t end() override;
        bool hasChild() override;
        void analyseType() override;
        std::string toString() override;

        std::shared_ptr<Identifier> name;
        std::vector<std::shared_ptr<Expression>> arguments;
        std::size_t lParenIndex;
        std::size_t rParenIndex;
    };



    class NumericLiteralExpression:public Expression{
    public:
        void forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream &ostream) override;
        SyntaxType getType() override;
        std::size_t start() override;
        std::size_t end() override;
        bool hasChild() override;
        void analyseType() override;
        std::string toString() override;

    
        std::size_t tokenIndex;
        std::string_view rawCode;
    };
    class ArrayInitializeExpression: public Expression, public ISyntaxList<Expression>{
    public:
        void analyseType() override;
        void add(const std::shared_ptr<Expression> &child) override;
        std::string toString() override;
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        std::size_t start() override;
        std::size_t end() override;
        SyntaxType getType() override;
        bool hasChild() override;

    
        std::size_t lBraceIndex, rBraceIndex;
        std::vector<std::shared_ptr<Expression>> array;
    };
}
