#pragma once

#include <memory>
#include "syntax_node.h"

namespace kisyshot::ast::syntax{
    class SyntaxOStreamWriter {
    public:
        static void writeTo(std::ostream &s, const std::weak_ptr<SyntaxNode> &node);
        static void writeTo(std::ostream &s, SyntaxNode &node);

    private:
        static void writeToChild(std::ostream &s, const std::weak_ptr<SyntaxNode> &node,
                                 std::string intend = "", bool isLast = true);
    };
    std::ostream &operator<<(std::ostream &ostream,SyntaxNode& node);
}
