#include <rang/rang.h>
#include <ast/syntax/syntax_node.h>
#include <ast/syntax/syntax_unit.h>

namespace kisyshot::ast::syntax {

    SyntaxType SyntaxUnit::getType() {
        return SyntaxType::SyntaxUnit;
    }

    bool SyntaxUnit::hasChild() {
        return !_syntax.empty();
    }

    void SyntaxUnit::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::gray << "SyntaxUnit "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::reset << std::endl;
        } else {
            s << "SyntaxUnit "
              << "<" << this << "> " << std::endl;
        }
    }

    void SyntaxUnit::add(const std::shared_ptr<VarDeclaration> &child) {
        _syntax.push_back(child);
    }

    void SyntaxUnit::add(const std::shared_ptr<Function> &child) {
        _syntax.push_back(child);
    }

    void SyntaxUnit::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        for (std::size_t i = 0; i < _syntax.size(); ++i) {
            syntaxWalker(_syntax[i], i == _syntax.size() - 1);
        }
    }

    std::size_t SyntaxUnit::start() {
        if (_syntax.empty())
            return invalidTokenIndex;
        return _syntax.front()->start();
    }

    std::size_t SyntaxUnit::end() {
        if (_syntax.empty())
            return invalidTokenIndex;
        return _syntax.back()->end();
    }
}