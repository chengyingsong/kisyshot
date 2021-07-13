#pragma once

#include "expression.h"
#include "statement.h"
#include "type.h"

namespace kisyshot::ast::syntax {
    class VarDefinition:public SyntaxNode, public ISyntaxList<Expression>{

        std::shared_ptr<Identifier> varName;
        std::shared_ptr<Expression> initialValue;
        std::vector<std::shared_ptr<Expression>> array;
        std::size_t equalTokenIndex;
    public:
        explicit VarDefinition(const std::shared_ptr<Identifier> &name);
        void add(const std::shared_ptr<Expression> &child) override;
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override;
        bool hasChild() override;
        std::string toString();
        std::size_t start() override;
        std::size_t end() override;
        void setInitialValue(const std::shared_ptr<Expression> &initialValue);
        void setEqualTokenIndex(size_t equalTokenIndex);

        const std::shared_ptr<Identifier> &getVarName() const;

        const std::shared_ptr<Expression> &getInitialValue() const;

        const std::vector<std::shared_ptr<Expression>> &getArray() const;

        size_t getEqualTokenIndex() const;
    };
    class VarDeclaration:public Statement, public ISyntaxList<VarDefinition>{

        std::shared_ptr<Type> type;
        std::vector<std::shared_ptr<VarDefinition>> varDefs;
        std::size_t constTokenIndex;
        std::size_t semiTokenIndex;
    public:
        void setSemiTokenIndex(size_t semiTokenIndex);
        VarDeclaration(const std::shared_ptr<Type> &type, size_t constIndex = invalidTokenIndex);
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void add(const std::shared_ptr<VarDefinition> &child) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override;
        bool hasChild() override;
        std::size_t start() override;
        std::size_t end() override;

        std::shared_ptr<Type> getVarType() const;
        const std::vector<std::shared_ptr<VarDefinition>> &getVarDefs() const;
        size_t getConstTokenIndex() const;
        size_t getSemiTokenIndex() const;
    };
}