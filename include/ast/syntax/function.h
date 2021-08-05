#pragma once

#include "var_declaration.h"
#include "statement.h"
#include "type.h"

namespace kisyshot::ast::syntax {
    class Function: public SyntaxNode{
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        bool hasChild() override ;
        SyntaxType getType() override ;
        std::size_t start() override;
        std::size_t end() override;
        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;

        std::shared_ptr<Type> returnType = nullptr;
        std::shared_ptr<Identifier> name = nullptr;
        std::shared_ptr<BlockStatement> body = nullptr;
        std::vector<std::shared_ptr<VarDefinition>> params;
        std::size_t lParenIndex = invalidTokenIndex;
        std::size_t rParenIndex = invalidTokenIndex;
        std::size_t stackSize = 0;
    };
}