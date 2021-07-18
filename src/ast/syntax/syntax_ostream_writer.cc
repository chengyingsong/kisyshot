
#include <ostream>
#include <rang/rang.h>
#include <ast/syntax/syntax_node.h>
#include <ast/syntax/syntax_ostream_writer.h>

namespace kisyshot::ast::syntax {
    void SyntaxOStreamWriter::writeTo(std::ostream &s, const std::weak_ptr<SyntaxNode> &node) {
        auto nLock = node.lock();
        nLock->writeCurrentInfo(s);
        nLock->forEachChild([&](const std::weak_ptr<SyntaxNode> &n, bool isChildLast) -> void {
            writeToChild(s, n, "", isChildLast);
        });
    }

    void SyntaxOStreamWriter::writeToChild(std::ostream &s, const std::weak_ptr<SyntaxNode> &node, std::string intend,
                                           bool isLast) {
        bool toConsole = s.rdbuf() == std::cout.rdbuf();

        if (toConsole) {
            s << rang::fg::blue << intend << (isLast ? "`-" : "|-") << rang::fg::reset;
        } else {
            s << intend << (isLast ? "`-" : "|-");
        }
        intend += isLast ? "  " : "| ";
        auto nLock = node.lock();
        nLock->writeCurrentInfo(s);
        nLock->forEachChild([&](const std::weak_ptr<SyntaxNode> &n, bool isChildLast) -> void {
            writeToChild(s, n, intend, isChildLast);
        });
    }

    void SyntaxOStreamWriter::writeTo(std::ostream &s, SyntaxNode &node) {
        node.writeCurrentInfo(s);
        node.forEachChild([&](const std::weak_ptr<SyntaxNode> &n, bool isChildLast) -> void {
            writeToChild(s, n, "", isChildLast);
        });
    }


    std::ostream &operator<<(std::ostream &ostream, SyntaxNode &node) {
        SyntaxOStreamWriter::writeTo(ostream, node);
        return ostream;
    }
}
