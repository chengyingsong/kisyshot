#include <rang/rang.h>
#include <ast/syntax/identifier.h>

namespace kisyshot::ast::syntax {

    Identifier::Identifier(const std::string &id, std::size_t tokId) {
        _identifier = id;
        _tokenId = tokId;
    }

    std::string Identifier::toString() {
        return _identifier;
    }

    void Identifier::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {}

    void Identifier::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::gray << "Identifier "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::green << "'" << _identifier << "'"
              << rang::fg::reset << std::endl;
        } else {
            s << "Identifier "
              << "<" << this << "> "
              << "'" << _identifier << "'" << std::endl;
        }
    }

    SyntaxType Identifier::getType() {
        return SyntaxType::Identifier;
    }

    bool Identifier::hasChild() {
        return false;
    }

    std::size_t Identifier::start() {
        return _tokenId;
    }

    std::size_t Identifier::end() {
        return _tokenId;
    }
}