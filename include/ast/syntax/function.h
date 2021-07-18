#pragma once

#include "param_list.h"
#include "statement.h"
#include "type.h"

namespace kisyshot::ast::syntax {
    class Function: public SyntaxNode {
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        bool hasChild() override ;
        SyntaxType getType() override ;
        std::size_t start() override;
        std::size_t end() override;
    
        std::shared_ptr<Type> returnType = nullptr;
        std::shared_ptr<Identifier> name = nullptr;
        std::shared_ptr<ParamList> params = nullptr;
        std::shared_ptr<Statement> body = nullptr;
        std::size_t lParenIndex = invalidTokenIndex;
        std::size_t rParenIndex = invalidTokenIndex;
    };
}