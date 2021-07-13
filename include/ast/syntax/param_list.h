#pragma once

#include "syntax_node.h"
#include "type.h"

namespace kisyshot::ast::syntax{
    class ParamDeclaration:public SyntaxNode{
    public:
        ParamDeclaration(const std::shared_ptr<Type>& type, const std::shared_ptr<Identifier> &name, size_t dim = 0);
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::weak_ptr<Identifier> getParamName();
        std::weak_ptr<Type> getParamType();
        std::size_t start() override;
        std::size_t end() override;

        size_t getDimension() const;

        const std::shared_ptr<Identifier> &getName() const;


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
