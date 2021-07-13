#pragma once

#include "param_list.h"
#include "statement.h"
#include "type.h"

namespace kisyshot::ast::syntax {
    class Function: public SyntaxNode {
    public:
        Function(const std::shared_ptr<Type>& returnType,
                          const std::shared_ptr<Identifier>& name,
                          const std::shared_ptr<ParamList>& params,
                          const std::shared_ptr<Statement>& body);
        void setLParenIndex(std::size_t lParenIndex);
        void setRParenIndex(std::size_t rParenIndex);
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        bool hasChild() override ;
        SyntaxType getType() override ;
        std::size_t start() override;
        std::size_t end() override;
        const std::shared_ptr<Type> &getReturnType() const;
        const std::shared_ptr<Identifier> &getName() const;
        const std::shared_ptr<ParamList> &getParams() const;
        const std::shared_ptr<Statement> &getBody() const;
        size_t getLParenIndex() const;
        size_t getRParenIndex() const;

    
        std::shared_ptr<Type> _returnType;
        std::shared_ptr<Identifier> _name;
        std::shared_ptr<ParamList> _params;
        std::shared_ptr<Statement> _body;
        std::size_t _lParenIndex;
        std::size_t _rParenIndex;
    };
}