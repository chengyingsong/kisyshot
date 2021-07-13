#pragma once

#include <string>
#include "syntax_node.h"

namespace kisyshot::ast::syntax{
    class Identifier:public SyntaxNode{
    public:
        explicit Identifier(const std::string& id, std::size_t tokenId);
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override;
        std::string toString() ;

        std::string _identifier;
        std::size_t _tokenId;
    };
}
