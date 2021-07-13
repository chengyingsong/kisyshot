#include <rang/rang.h>
#include <ast/syntax/param_list.h>

namespace kisyshot::ast::syntax {


    ParamDeclaration::ParamDeclaration(const std::shared_ptr<Type> &type, const std::shared_ptr<Identifier> &name,
                                       size_t dim) {
        this->name = name;
        this->type = type;
        this->dimension = dim;
    }

    void ParamDeclaration::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(name, false);
        syntaxWalker(type, true);
    }

    void ParamDeclaration::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::gray << "ParamDeclaration "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::cyan << "'" << type->toString() << " " << name->toString();

            s << "'"
              << rang::fg::reset << std::endl;
        } else {
            s << "ParamDeclaration "
              << "<" << this << "> "
              << "'" << type->toString() << " " << name->toString();
            s << "'" << std::endl;
        }
    }

    SyntaxType ParamDeclaration::getType() {
        return SyntaxType::ParamDeclaration;
    }

    bool ParamDeclaration::hasChild() {
        return true;
    }

    std::weak_ptr<Identifier> ParamDeclaration::getParamName() {
        return name;
    }

    std::weak_ptr<Type> ParamDeclaration::getParamType() {
        return type;
    }

    std::size_t ParamDeclaration::start() {
        return type->start();
    }

    std::size_t ParamDeclaration::end() {
        return name->end();
    }

    size_t ParamDeclaration::getDimension() const {
        return dimension;
    }

    const std::shared_ptr<Identifier> &ParamDeclaration::getName() const {
        return name;
    }

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
}