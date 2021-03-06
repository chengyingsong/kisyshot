#include <rang/rang.h>
#include <memory>
#include <sstream>
#include <ast/syntax/type.h>

namespace kisyshot::ast::syntax{
    void Type::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(typeName, true);
    }

    void Type::writeCurrentInfo(std::ostream &s) {
        if(s.rdbuf() == std::cout.rdbuf()){
            s << rang::fg::gray    << "Type "
              << rang::fg::yellow  << "<" << this << "> "
              << rang::fg::green   << "'"  << toString()  << "'"
              << rang::fg::reset << std::endl;
        } else{
            s << "Type "
              << "<" << this << "> "
              << "'"  << toString()  << "'" << std::endl;
        }
    }

    std::string Type::toString() {
        return typeName->toString();
    }

    SyntaxType Type::getType() {
        return SyntaxType::Type;
    }

    bool Type::hasChild() {
        return false;
    }

    std::size_t Type::start() {
        return typeName->start();
    }

    std::size_t Type::end() {
        return typeName->end();
    }

    void Type::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {}

}