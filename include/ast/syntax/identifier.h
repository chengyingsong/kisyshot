#pragma once

#include <string>
#include "syntax_node.h"

namespace kisyshot::ast::syntax{
    class Identifier:public SyntaxNode{
    public:
        void forEachChild(const std::function<void (std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) override;
        void writeCurrentInfo(std::ostream& ostream) override;
        SyntaxType getType() override ;
        bool hasChild() override ;
        std::size_t start() override ;
        std::size_t end() override;
        std::string toString() ;

        void genCode(compiler::CodeGenerator &gen,ast::Var* temp) override;

        std::string identifier;
        std::string mangledId;
        std::size_t tokenIndex = invalidTokenIndex;
    };
}
