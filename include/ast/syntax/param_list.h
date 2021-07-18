#pragma once

#include "syntax_node.h"
#include "type.h"

namespace kisyshot::ast::syntax{
    class ParamDeclaration:public SyntaxNode{
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override;
        std::size_t end() override;

        std::size_t dimension;
        std::shared_ptr<Identifier> name;
        std::shared_ptr<Type> type;
    };
    class ParamList: public SyntaxNode, public ISyntaxList<ParamDeclaration>{
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        void add(const std::shared_ptr<ParamDeclaration>& param) override ;
        std::size_t start() override;
        std::size_t end() override;

        std::vector<std::shared_ptr<ParamDeclaration>> params;
    };
}
