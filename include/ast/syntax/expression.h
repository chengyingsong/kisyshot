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
        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override = 0;
        Var* getVar(compiler::CodeGenerator &gen);
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

        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;
        std::shared_ptr<Expression> left = nullptr;
        std::shared_ptr<Expression> right = nullptr;
        TokenType operatorType = TokenType::undefined;
        std::size_t opIndex = invalidTokenIndex;
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

        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;

        TokenType operatorType = TokenType::undefined;
        std::size_t opIndex = invalidTokenIndex;
        std::shared_ptr<Expression> right = nullptr;
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

        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;
        std::shared_ptr<Identifier> name = nullptr;
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

        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;
        std::size_t leftParenIndex = invalidTokenIndex;
        std::size_t rightParenIndex = invalidTokenIndex;
        std::shared_ptr<Expression> innerExpression = nullptr;
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
        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;
        std::shared_ptr<Identifier> arrayName = nullptr;
        std::shared_ptr<Expression> indexedExpr = nullptr;
        std::shared_ptr<Expression> indexerExpr = nullptr;
        Var* offset;
        bool isStore= false;
        bool isOutSideLayer = false;
        std::size_t layer = 1;
        std::size_t lSquareIndex = invalidTokenIndex;
        std::size_t rSquareIndex = invalidTokenIndex;
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

        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;
        std::shared_ptr<Identifier> name = nullptr;
        std::vector<std::shared_ptr<Expression>> arguments;
        std::size_t lParenIndex = invalidTokenIndex;
        std::size_t rParenIndex = invalidTokenIndex;
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
        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;
        std::size_t tokenIndex;
        std::string_view rawCode;
    };

    class StringLiteralExpression:public Expression{
    public:
        void forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream &ostream) override;
        SyntaxType getType() override;
        std::size_t start() override;
        std::size_t end() override;
        bool hasChild() override;
        void analyseType() override;
        std::string toString() override;
        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;
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
        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;
        std::size_t lBraceIndex, rBraceIndex;
        std::vector<std::shared_ptr<Expression>> array;
    };
}
