#include <rang/rang.h>
#include <ast/syntax/param_list.h>

namespace kisyshot::ast::syntax {


    ParamDeclaration::ParamDeclaration(const std::shared_ptr<Type> &type, const std::shared_ptr<Identifier> &name,
                                       size_t dim) {
        _name = name;
        _type = type;
        _dimension = dim;
    }

    void ParamDeclaration::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(_name, false);
        syntaxWalker(_type, true);
    }

    void ParamDeclaration::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::gray << "ParamDeclaration "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::cyan << "'" << _type->toString() << " " << _name->toString();

            s << "'"
              << rang::fg::reset << std::endl;
        } else {
            s << "ParamDeclaration "
              << "<" << this << "> "
              << "'" << _type->toString() << " " << _name->toString();
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
        return _name;
    }

    std::weak_ptr<Type> ParamDeclaration::getParamType() {
        return _type;
    }

    std::size_t ParamDeclaration::start() {
        return _type->start();
    }

    std::size_t ParamDeclaration::end() {
        return _name->end();
    }

    size_t ParamDeclaration::getDimension() const {
        return _dimension;
    }

    const std::shared_ptr<Identifier> &ParamDeclaration::getName() const {
        return _name;
    }

    void ParamList::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        for (std::size_t i = 0; i < _params.size(); i++) {
            syntaxWalker(_params[i], i == _params.size() - 1);
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
        return !_params.empty();
    }

    void ParamList::add(const std::shared_ptr<ParamDeclaration> &param) {
        _params.push_back(param);
    }

    std::size_t ParamList::start() {
        if (_params.empty())
            return invalidTokenIndex;
        return _params.front()->start();
    }

    std::size_t ParamList::end() {
        if (_params.empty())
            return invalidTokenIndex;
        return _params.back()->end();
    }
}