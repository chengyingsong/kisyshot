#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <vector>
#include<compiler/codegen.h>

namespace kisyshot::ast::syntax{
    enum class SyntaxType{
        SyntaxUnit,
        VariableDeclaration,
        VariableDefinition,

        BlockStatement,
        IfStatement,
        WhileStatement,
        NopStatement,
        BreakStatement,
        ContinueStatement,
        ReturnStatement,
        ExpressionStatement,

        Function,
        ParamDeclaration,
        ParamList,

        Identifier,

        Type,

        ArrayInitializeExpression,
        BinaryExpression,
        UnaryExpression,
        IdentifierExpression,
        NumericLiteralExpression,
        CallExpression,
        IndexExpression,
        ParenthesesExpression,


        InlineComment,
        InterlineComment
    };
    std::ostream & operator<<(std::ostream& s,SyntaxType type);

    //class ISyntaxContext{};

    class SyntaxNode{
    public:
        virtual void forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>,bool)>& syntaxWalker) = 0;
        virtual void writeCurrentInfo(std::ostream& ostream) = 0;
        virtual SyntaxType getType() = 0;
        virtual bool hasChild() = 0;
        virtual std::size_t start() = 0;
        virtual std::size_t end() = 0;
        virtual ~SyntaxNode() = default;
        //需要生成中间代码的节点实现该方法
        virtual void genCode(compiler::CodeGenerator &gen,ast::Var* temp) = 0;
        constexpr static std::size_t invalidTokenIndex = std::numeric_limits<std::size_t>::max();
    };

    template <typename TListElem>
    class ISyntaxList{
    public:
        virtual void add(const std::shared_ptr<TListElem>& child) = 0;
        virtual ~ISyntaxList();
    };

    template<typename TListElem>
    ISyntaxList<TListElem>::~ISyntaxList() {
        static_assert(std::is_base_of<SyntaxNode,TListElem>::value);
    }

}