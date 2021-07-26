#pragma once

#include "expression.h"
#include "statement.h"
#include "type.h"


//TODO::思考一下中间代码生成中变量声明的处理方法

namespace kisyshot::ast::syntax {
    class VarDefinition:public SyntaxNode, public ISyntaxList<Expression>{
    public:
        void add(const std::shared_ptr<Expression> &child) override;
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override;
        bool hasChild() override;
        std::string toString();
        std::size_t start() override;
        std::size_t end() override;
        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;

        std::shared_ptr<Type> type = nullptr;
        std::shared_ptr<Identifier> varName = nullptr;
        std::shared_ptr<Expression> initialValue = nullptr;
        std::vector<std::shared_ptr<Expression>> array;
        std::size_t equalTokenIndex = invalidTokenIndex;
    };
    class VarDeclaration:public Statement, public ISyntaxList<VarDefinition>{

    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void add(const std::shared_ptr<VarDefinition> &child) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override;
        bool hasChild() override;
        std::size_t start() override;
        std::size_t end() override;
        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;

        std::size_t offset;
        std::shared_ptr<Type> type = nullptr;
        std::vector<std::shared_ptr<VarDefinition>> varDefs ;
        std::size_t constTokenIndex = invalidTokenIndex;
        std::size_t semiTokenIndex = invalidTokenIndex;
    };
}