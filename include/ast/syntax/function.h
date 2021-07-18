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
    
        std::shared_ptr<Type> returnType;
        std::shared_ptr<Identifier> name;
        std::shared_ptr<ParamList> params;
        std::shared_ptr<Statement> body;
        std::size_t lParenIndex;
        std::size_t rParenIndex;
    };
}