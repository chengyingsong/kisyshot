#pragma once

#include "expression.h"
#include "statement.h"
#include "type.h"

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

        std::shared_ptr<Identifier> varName;
        std::shared_ptr<Expression> initialValue;
        std::vector<std::shared_ptr<Expression>> array;
        std::size_t equalTokenIndex;
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

        std::shared_ptr<Type> type;
        std::vector<std::shared_ptr<VarDefinition>> varDefs;
        std::size_t constTokenIndex;
        std::size_t semiTokenIndex;
    };
}