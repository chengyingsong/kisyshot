#include <rang/rang.h>
#include <ast/syntax/comment.h>

namespace kisyshot::ast::syntax {

    SyntaxType InlineComment::getType() {
        return SyntaxType::InlineComment;
    }

    bool InlineComment::hasChild() {
        return false;
    }

    void InlineComment::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::gray << "InlineComment "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::reset << std::endl;
        } else {
            s << "InlineComment "
              << "<" << this << "> " << std::endl;
        }
    }

    void InlineComment::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {}

    SyntaxType InterlineComment::getType() {
        return SyntaxType::InterlineComment;
    }

    bool InterlineComment::hasChild() {
        return false;
    }

    void InterlineComment::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::gray << "InterlineComment "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::reset << std::endl;
        } else {
            s << "InterlineComment "
              << "<" << this << "> " << std::endl;
        }
    }

    void InterlineComment::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {}
}