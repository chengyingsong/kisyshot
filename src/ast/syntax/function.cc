#include <sstream>
#include <memory>
#include <rang/rang.h>
#include <compiler/codegen.h>
#include <ast/syntax/function.h>

namespace kisyshot::ast::syntax {
    void Function::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(name, false);
        syntaxWalker(returnType, false);
        syntaxWalker(params, false);
        syntaxWalker(body, true);
    }

    void Function::genCode(compiler::CodeGenerator &gen,ast::Var* temp) {
        //gen.genFuncName(name)
        std::string funName = "." + name->identifier + ":";
        gen.genLabel(funName);
        gen.genBeginFunc();
        body->genCode(gen, nullptr); //body是Statement类型
        gen.genEndFunc();
    }

    void Function::writeCurrentInfo(std::ostream &s) {
        std::ostringstream methodData;
        methodData << "'" << returnType->toString() << " " << name->toString();

        methodData << "(";
        if (params->hasChild()) {
            params->forEachChild([&methodData](const std::weak_ptr<SyntaxNode> &n, bool isLast) {
                auto para = std::static_pointer_cast<ParamDeclaration>(n.lock());
                methodData << para->toString()
                           << (isLast ? ")'" : ", ");
            });
        } else {
            methodData << ")'";
        }
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::gray << "Function "
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::green << methodData.str()
              << rang::fg::reset << std::endl;
        } else {
            s << "Function "
              << "<" << this << "> "
              << methodData.str() << std::endl;
        }
    }

    bool Function::hasChild() {
        return true;
    }

    SyntaxType Function::getType() {
        return SyntaxType::Function;
    }

    std::size_t Function::start() {
        return returnType->start();
    }

    std::size_t Function::end() {
        if (body->getType() == SyntaxType::BlockStatement)
            return body->end();
        if (body->end() != invalidTokenIndex)
            return body->end();
        if (rParenIndex != invalidTokenIndex)
            return rParenIndex;
        if (params->hasChild())
            return params->end();
        if (lParenIndex != invalidTokenIndex)
            return lParenIndex;
        return name->end();
    }

}