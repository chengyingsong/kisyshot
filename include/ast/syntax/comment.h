#pragma once

#include "syntax_node.h"

namespace kisyshot::ast::syntax{
    class InlineComment:public SyntaxNode{
        SyntaxType getType() override;
        bool hasChild() override;
        void writeCurrentInfo(std::ostream &ostream) override;
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
    };

    class InterlineComment:public SyntaxNode{
        SyntaxType getType() override;
        bool hasChild() override;
        void writeCurrentInfo(std::ostream &ostream) override;
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
    };
}
