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
        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override = 0;
    };

    class IfStatement:public Statement{
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override ;
        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;
    
        std::shared_ptr<Statement> ifClause = nullptr;
        std::shared_ptr<Statement> elseClause = nullptr;
        std::shared_ptr<Expression> condition = nullptr;
        std::size_t ifTokenIndex = invalidTokenIndex;
        std::size_t elseTokenIndex = invalidTokenIndex;
        std::size_t lParenIndex = invalidTokenIndex;
        std::size_t rParenIndex = invalidTokenIndex;
    };


    class WhileStatement:public Statement{
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override ;
        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;
    
        std::size_t whileTokenIndex = invalidTokenIndex;
        std::size_t lParenIndex = invalidTokenIndex;
        std::size_t rParenIndex = invalidTokenIndex;
        std::shared_ptr<Expression> condition = nullptr;
        std::shared_ptr<Statement> body = nullptr;
    };


    class NopStatement:public Statement{
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override ;
        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override; //不实现
    
        std::size_t tokenIndex = invalidTokenIndex;
    };

    //TODO: 支持break和continue
    class BreakStatement:public Statement{
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override ;

        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;
        std::size_t breakTokenIndex = invalidTokenIndex;
        std::size_t semiTokenIndex = invalidTokenIndex;
    };

    class ContinueStatement:public Statement{
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override ;

        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;
        std::size_t continueTokenIndex = invalidTokenIndex;
        std::size_t semiTokenIndex = invalidTokenIndex;
    };

    class ReturnStatement:public Statement{
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override ;

        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;
        std::size_t returnTokenIndex = invalidTokenIndex;
        std::size_t semiTokenIndex = invalidTokenIndex;
        std::shared_ptr<Expression> value = nullptr;
    };

    class BlockStatement: public ISyntaxList<Statement>,
                          public Statement{
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override ;
        void add(const std::shared_ptr<Statement>& child) override ;

        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;
        std::vector<std::shared_ptr<Statement>> children;
        std::size_t lBraceTokenIndex = invalidTokenIndex;
        std::size_t rBraceTokenIndex = invalidTokenIndex;
    };

    class ExpressionStatement: public Statement{
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override ;
        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;
    
        std::shared_ptr<Expression> expression = nullptr;
        std::size_t semiTokenIndex = invalidTokenIndex;
    };
}