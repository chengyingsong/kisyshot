#include <rang/rang.h>
#include <ast/syntax/identifier.h>

namespace kisyshot::ast::syntax {
    std::string Identifier::toString() {
        return identifier;
    }

    void Identifier::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {}

    void Identifier::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::gray << "Identifier "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::green << "'" << identifier << "'" << " (a.k.a '" << mangledId << "')"
              << rang::fg::reset << std::endl;
        } else {
            s << "Identifier "
              << "<" << this << "> "
              << "'" << identifier << "'" << " (a.k.a '" << mangledId << "')" << std::endl;
        }
    }

    SyntaxType Identifier::getType() {
        return SyntaxType::Identifier;
    }

    bool Identifier::hasChild() {
        return false;
    }

    std::size_t Identifier::start() {
        return tokenIndex;
    }

    std::size_t Identifier::end() {
        return tokenIndex;
    }

    void Identifier::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {}
}