#include <rang/rang.h>
#include <ast/syntax/param_list.h>

namespace kisyshot::ast::syntax {
    void ParamDeclaration::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(name, false);
        syntaxWalker(type, true);
    }

    void ParamDeclaration::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::gray << "ParamDeclaration "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::cyan << "'" << toString() << "'"
              << rang::fg::reset << std::endl;
        } else {
            s << "ParamDeclaration "
              << "<" << this << "> "
              << "'" << toString() << "'" << std::endl;
        }
    }

    SyntaxType ParamDeclaration::getType() {
        return SyntaxType::ParamDeclaration;
    }

    bool ParamDeclaration::hasChild() {
        return true;
    }

    std::size_t ParamDeclaration::start() {
        return type->start();
    }

    std::size_t ParamDeclaration::end() {
        return name->end();
    }

    std::string ParamDeclaration::toString() const {
        std::string s = type->toString() + " " + name->toString();
        for (int i = 0; i < dimension; ++i) {
            s += "[]";
        }
        return s;
    }

    void ParamDeclaration::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {}

    void ParamList::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        for (std::size_t i = 0; i < params.size(); i++) {
            syntaxWalker(params[i], i == params.size() - 1);
        }
    }

    void ParamList::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::gray << "ParamList "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::reset << std::endl;
        } else {
            s << "ParamList "
              << "<" << this << "> " << std::endl;
        }
    }

    SyntaxType ParamList::getType() {
        return SyntaxType::ParamList;
    }

    bool ParamList::hasChild() {
        return !params.empty();
    }

    void ParamList::add(const std::shared_ptr<ParamDeclaration> &param) {
        params.push_back(param);
    }

    std::size_t ParamList::start() {
        if (params.empty())
            return invalidTokenIndex;
        return params.front()->start();
    }

    std::size_t ParamList::end() {
        if (params.empty())
            return invalidTokenIndex;
        return params.back()->end();
    }

    void ParamList::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {}
}