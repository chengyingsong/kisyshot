#include <rang/rang.h>
#include <ast/syntax/syntax_node.h>
#include <ast/syntax/syntax_unit.h>

namespace kisyshot::ast::syntax {

    SyntaxType SyntaxUnit::getType() {
        return SyntaxType::SyntaxUnit;
    }

    bool SyntaxUnit::hasChild() {
        return !children.empty();
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
        children.push_back(child);
    }

    void SyntaxUnit::add(const std::shared_ptr<Function> &child) {
        children.push_back(child);
    }

    void SyntaxUnit::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        for (std::size_t i = 0; i < children.size(); ++i) {
            syntaxWalker(children[i], i == children.size() - 1);
        }
    }

    std::size_t SyntaxUnit::start() {
        if (children.empty())
            return invalidTokenIndex;
        return children.front()->start();
    }

    std::size_t SyntaxUnit::end() {
        if (children.empty())
            return invalidTokenIndex;
        return children.back()->end();
    }

    void SyntaxUnit::genCode(compiler::CodeGenerator &gen,ast::Var* temp) {
        //根结点生成中间代码，直接调用子节点的方法即可
        for(std::size_t i = 0; i < children.size(); ++i) {
            children[i]->genCode(gen, nullptr);
        }
    }
}