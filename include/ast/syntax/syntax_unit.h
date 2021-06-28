#pragma once

#include "function.h"
#include "var_declaration.h"

namespace kisyshot::ast::syntax{
    class SyntaxUnit: public SyntaxNode,
                      public ISyntaxList<VarDeclaration>,
                      public ISyntaxList<Function>{
    public:
        SyntaxType getType() override;
        bool hasChild() override;
        void writeCurrentInfo(std::ostream &ostream) override;
        void add(const std::shared_ptr<VarDeclaration> &child) override;
        void add(const std::shared_ptr<Function> &child) override;
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        std::size_t start() override;
        std::size_t end() override;
    private:
        std::vector<std::shared_ptr<SyntaxNode>> _syntax;
    };
}
