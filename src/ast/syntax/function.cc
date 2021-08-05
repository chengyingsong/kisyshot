#include <sstream>
#include <memory>
#include <rang/rang.h>
#include <compiler/codegen.h>
#include <ast/syntax/function.h>

namespace kisyshot::ast::syntax {
    void Function::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(name, false);
        syntaxWalker(returnType, false);
        for (size_t i = 0; i < params.size(); i++) {
            syntaxWalker(params[i], (i == params.size() - 1) && body == nullptr);
        }
        if (body != nullptr) {
            syntaxWalker(body, true);
        }
    }

    void Function::genCode(compiler::CodeGenerator &gen,ast::Var* temp) {
        //gen.genFuncName(name)
        if (body != nullptr) {
            for (auto &param : params) {
                //TODO:要不要声明一下参数呀
                    param->genCode(gen, nullptr);
            }

            std::string funName = name->identifier;
            std::string endFuncLabel = gen.newLabel();
            gen.genLabel(funName);
            gen.genBeginFunc(stackSize);  //设置栈帧写在beginFunc中
            body->endFuncLabel = endFuncLabel;
            body->genCode(gen, nullptr); //body是Statement类型
            gen.genLabel(endFuncLabel);
            gen.genEndFunc();
        }
    }

    void Function::writeCurrentInfo(std::ostream &s) {
        std::ostringstream methodData;
        methodData << "'" << returnType->toString() << " " << name->toString();

        methodData << "(";
        for (auto &param : params) {
            methodData << param->type->toString() << " " << param->toString();
            if (param != params.back()) 
                methodData << ", ";
        }
        methodData << ")'";
        
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
        if (!params.empty())
            return params.back()->end();
        if (lParenIndex != invalidTokenIndex)
            return lParenIndex;
        return name->end();
    }

}