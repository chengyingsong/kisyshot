#include <utility>
#include <rang/rang.h>
#include <ast/syntax/expression.h>

namespace kisyshot::ast::syntax {
    void BinaryExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(left, false);
        syntaxWalker(right, true);
    }

    void BinaryExpression::writeCurrentInfo(std::ostream &s) {
        Expression::writeCurrentInfo(s);
    }

    void BinaryExpression::analyseType() {

    }

    SyntaxType BinaryExpression::getType() {
        return SyntaxType::BinaryExpression;
    }

    bool BinaryExpression::hasChild() {
        return true;
    }

    std::size_t BinaryExpression::start() {
        return left->start();
    }

    std::size_t BinaryExpression::end() {
        if (right != nullptr)
            return right->end();
        return opIndex;
    }

    std::string BinaryExpression::toString() {
        return left->toString() + " " + getTokenSpell(operatorType) + " " + right->toString();
    }

    void BinaryExpression::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {

        //两个表达式类型的子类
        //TODO: 支持逻辑表达式
        std::string opName = getTokenSpell(operatorType);
        if(opName == "="){
            Var* src_1 = left->getVar(gen);
            Var* src_2 = right->getVar(gen);
            //left是左值
            gen.genAssign(src_2,src_1); 
        } else if(opName == "&&"){
            // temp = src_1 && src_2
            /*  src_1
             * ifZ src_1 GOTO falseLabel
             *  src_2
             *  IfZ src_2 GOTO falseLabel
             *  temp = 1
             *  Goto endLabel
             *  falseLabel:
             *  temp = 0
             *  endLabel:
             * */
            Var * t0 =  gen.getConstVar(0);
            Var * t1 =  gen.getConstVar(1);
            std::string  falseLabel = gen.newLabel();
            std::string  endLabel = gen.newLabel();
            Var* src_1 = left->getVar(gen);
            gen.genIFZ(src_1,falseLabel);
            Var* src_2 = right->getVar(gen);
            gen.genIFZ(src_2,falseLabel);
            gen.genAssign(t1,temp);
            gen.genGOTO(endLabel);
            gen.genLabel(falseLabel);
            gen.genAssign(t0,temp);
            gen.genLabel(endLabel);
        } else if(opName == "||"){
            /* temp = src_1 || src_2
             * src_1
             * ifZ src_1 GOTO next
             * GOTO trueLabel
             * next:
             * src_2
             * ifZ src_2 GOTO falseLabel
             * trueLabel:
             * temp = 1
             * GOTO endLabel
             * falseLabel:
             * temp = 0
             * endLabel
             * */

            Var * t0 =  gen.getConstVar(0);
            Var * t1 =  gen.getConstVar(1);
            std::string next = gen.newLabel();
            std::string trueLabel = gen.newLabel();
            std::string falseLabel = gen.newLabel();
            std::string endLabel = gen.newLabel();
            Var* src_1 = left->getVar(gen);
            gen.genIFZ(src_1,next);
            gen.genGOTO(trueLabel);
            gen.genLabel(next);
            Var* src_2 = right->getVar(gen);
            gen.genIFZ(src_2,falseLabel);
            gen.genLabel(trueLabel);
            gen.genAssign(t1,temp);
            gen.genGOTO(endLabel);
            gen.genLabel(falseLabel);
            gen.genAssign(t0,temp);
            gen.genLabel(endLabel);
        }
        else{
            Var* src_1 = left->getVar(gen);
            Var* src_2 = right->getVar(gen);
            gen.genBinaryOp(opName, src_1, src_2,temp);
        }
    }

    void UnaryExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(right, true);
    }

    void UnaryExpression::writeCurrentInfo(std::ostream &s) {
        Expression::writeCurrentInfo(s);
    }

    void UnaryExpression::analyseType() {

    }

    SyntaxType UnaryExpression::getType() {
        return SyntaxType::UnaryExpression;
    }

    bool UnaryExpression::hasChild() {
        return true;
    }

    std::size_t UnaryExpression::start() {
        return opIndex;
    }

    std::size_t UnaryExpression::end() {
        if (right != nullptr)
            return right->end();
        return opIndex;
    }

    std::string UnaryExpression::toString() {
        return getTokenSpell(operatorType) + right->toString();
    }

    void UnaryExpression::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {
        //单目运算符 -
        Var* t = right->getVar(gen);
        //TODO: 支持！ 逻辑运算
        if(operatorType == TokenType::op_exclaim){
           /*ifZ t GOTO temp0
            * temp = 0
            * GOTO end
            * temp0:
            * temp = `
            * end
            * */
            std::string label0 = gen.newLabel();
            std::string endLabel = gen.newLabel();
            gen.genIFZ(t,label0);
            gen.genAssign(gen.getConstVar(0),temp);
            gen.genGOTO(endLabel);
            gen.genLabel(label0);
            gen.genAssign(gen.getConstVar(1),temp);
            gen.genLabel(endLabel);
        }else{  //负号
            Var* t0 = gen.getConstVar(0);
            std::string opName = "-";
            gen.genBinaryOp(opName,t0,t,temp);
        }

    }

    void IdentifierExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(name, true);
    }

    void IdentifierExpression::writeCurrentInfo(std::ostream &s) {
        Expression::writeCurrentInfo(s);
    }

    SyntaxType IdentifierExpression::getType() {
        return SyntaxType::IdentifierExpression;
    }

    bool IdentifierExpression::hasChild() {
        return true;
    }

    void IdentifierExpression::analyseType() {

    }

    std::size_t IdentifierExpression::start() {
        return name->start();
    }

    std::size_t IdentifierExpression::end() {
        return name->end();
    }

    std::string IdentifierExpression::toString() {
        return name->toString();
    }

    void IdentifierExpression::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {
        //单值表达式，首先查询变量表，然后返回绑定的变量
        Var* src_1  = temp;
        Var* src_2 = gen.name2VarMap[name->mangledId]; //由重整名获取变量
        gen.genAssign(src_2,src_1);
    }

    void Expression::writeCurrentInfo(std::ostream &s) {
        if (s.rdbuf() == std::cout.rdbuf()) {
            s << rang::fg::gray << getType()
              << rang::fg::yellow << "<" << this << "> "
              << rang::fg::green << "'" << toString() << "' "
              << rang::fg::reset << std::endl;
        } else {
            s << " " << getType()
              << "<" << this << "> "
              << "'" << toString() << "' " << std::endl;
        }
    }

    Var *Expression::getVar(compiler::CodeGenerator &gen) {
        Var *t = nullptr;
        switch(getType()){
            case SyntaxType::IdentifierExpression :
                t = gen.name2VarMap[((IdentifierExpression *) this)->name->mangledId];
                break;
            case SyntaxType::NumericLiteralExpression:
                t = gen.getConstVar(std::stoi(toString()));
                break;
            case SyntaxType::StringLiteralExpression: {
                t = new Var();
                t->type = VarType::StringVar;
                t->s = toString();
            }
                break;
            case SyntaxType::IndexExpression:{
                //TODO:数组引用的最外层,base是Index节点的数组名,在indexExpression中计算offset
                Var* t = gen.newTempVar();
                Var* base = gen.name2VarMap[((IdentifierExpression *) this)->name->mangledId];
                Var* offset = gen.newTempVar();  //计算offset
                genCode(gen, offset);
                gen.genLoad(base,offset,t);
            }
                break;
            default :
                t = gen.newTempVar();
                genCode(gen, t);
        }
        return t;
    }

    void ParenthesesExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(innerExpression, true);
    }

    void ParenthesesExpression::writeCurrentInfo(std::ostream &ostream) {
        Expression::writeCurrentInfo(ostream);
    }

    SyntaxType ParenthesesExpression::getType() {
        return SyntaxType::ParenthesesExpression;
    }

    std::size_t ParenthesesExpression::start() {
        return leftParenIndex;
    }

    std::size_t ParenthesesExpression::end() {
        if (rightParenIndex != invalidTokenIndex)
            return rightParenIndex;
        if (innerExpression != nullptr)
            return innerExpression->end();
        return leftParenIndex;
    }

    bool ParenthesesExpression::hasChild() {
        return true;
    }

    void ParenthesesExpression::analyseType() {

    }

    std::string ParenthesesExpression::toString() {
        return "(" + innerExpression->toString() + ")";
    }

    void ParenthesesExpression::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {
        //直接生成内部表达式
        innerExpression->genCode(gen,temp);
    }

    void IndexExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(indexedExpr, false);
        syntaxWalker(indexerExpr, true);
    }

    void IndexExpression::writeCurrentInfo(std::ostream &ostream) {
        Expression::writeCurrentInfo(ostream);
    }

    SyntaxType IndexExpression::getType() {
        return SyntaxType::IndexExpression;
    }

    std::size_t IndexExpression::start() {
        if (indexedExpr != nullptr)
            return indexedExpr->start();
        return lSquareIndex;
    }

    std::size_t IndexExpression::end() {
        if (rSquareIndex != invalidTokenIndex)
            return rSquareIndex;
        if (indexerExpr != nullptr)
            return indexerExpr->end();
        return lSquareIndex;
    }

    bool IndexExpression::hasChild() {
        return true;
    }

    void IndexExpression::analyseType() {

    }

    std::string IndexExpression::toString() {
        return indexedExpr->toString() + "[" + indexerExpr->toString() + "]";
    }

    void IndexExpression::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {
        //递归计算 a[i][j] = a[i] + j = a + i* dim1 + j
        //TODO: 获取数组名和数组每一维的维度信息，计算offset，
        //Var * offset = indexerExpr->getVar(gen);  //计算当前 offset
        // gen.genLoad(base,offset,temp);
    }

    void CallExpression::add(const std::shared_ptr<Expression> &child) {
        arguments.push_back(child);
    }

    void CallExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
        syntaxWalker(name, arguments.empty());
        for (std::size_t i = 0; i < arguments.size(); ++i) {
            syntaxWalker(arguments[i], i == arguments.size() - 1);
        }
    }

    void CallExpression::writeCurrentInfo(std::ostream &ostream) {
        Expression::writeCurrentInfo(ostream);
    }

    SyntaxType CallExpression::getType() {
        return SyntaxType::CallExpression;
    }

    std::size_t CallExpression::start() {
        return name->start();
    }

    std::size_t CallExpression::end() {
        if (rParenIndex != invalidTokenIndex)
            return rParenIndex;
        if (!arguments.empty()) {
            for (std::size_t i = 0; i <= arguments.size(); i++) {
                if (arguments[arguments.size() - i] != nullptr)
                    return arguments[arguments.size() - i]->end();
            }
        }
        return lParenIndex;
    }

    bool CallExpression::hasChild() {
        return true;
    }

    void CallExpression::analyseType() {

    }

    std::string CallExpression::toString() {
        std::string result = name->toString() + "(";
        for (std::size_t i = 0; i < arguments.size(); ++i) {
            result += arguments[i]->toString();
            if (i != arguments.size() - 1)
                result += ", ";
        }
        return result + ")";
    }

    void CallExpression::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {
        for(auto& argument: arguments) {
            Var *t = argument->getVar(gen);
           // argument->genCode(gen, t);  //参数声明语句
            gen.genParam(t);
        }
        std::string funName = name->toString();
        if(temp!= nullptr)
            gen.genCall(funName,arguments.size(),temp);
        else
            gen.genCallNoReturn(funName,arguments.size());
    }

    void
    NumericLiteralExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
    }

    void NumericLiteralExpression::writeCurrentInfo(std::ostream &ostream) {
        Expression::writeCurrentInfo(ostream);
    }

    SyntaxType NumericLiteralExpression::getType() {
        return SyntaxType::NumericLiteralExpression;
    }

    std::size_t NumericLiteralExpression::start() {
        return tokenIndex;
    }

    std::size_t NumericLiteralExpression::end() {
        return tokenIndex;
    }

    bool NumericLiteralExpression::hasChild() {
        return false;
    }

    void NumericLiteralExpression::analyseType() {

    }

    std::string NumericLiteralExpression::toString() {
        return (std::string) rawCode;
    }

    void NumericLiteralExpression::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {
        gen.genAssign((gen.getConstVar(std::stoi(toString()))),temp); 
    }


    void
    StringLiteralExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {
    }

    void StringLiteralExpression::writeCurrentInfo(std::ostream &ostream) {
        Expression::writeCurrentInfo(ostream);
    }

    SyntaxType StringLiteralExpression::getType() {
        return SyntaxType::StringLiteralExpression;
    }

    std::size_t StringLiteralExpression::start() {
        return tokenIndex;
    }

    std::size_t StringLiteralExpression::end() {
        return tokenIndex;
    }

    bool StringLiteralExpression::hasChild() {
        return false;
    }

    void StringLiteralExpression::analyseType() {

    }

    std::string StringLiteralExpression::toString() {
        return (std::string) rawCode;
    }

    void StringLiteralExpression::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {}

    void ArrayInitializeExpression::analyseType() {
    }

    std::string ArrayInitializeExpression::toString() {
        std::string result = "{ ";
        forEachChild([&result](const std::weak_ptr<SyntaxNode> &node, bool last) -> void {

            result += std::dynamic_pointer_cast<Expression>(node.lock())->toString();
            if (!last)
                result += ", ";
        });
        return result + " }";
    }

    void
    ArrayInitializeExpression::forEachChild(const std::function<void(std::weak_ptr<SyntaxNode>, bool)> &syntaxWalker) {

        for (size_t id = 0; id < array.size(); id++) {
            syntaxWalker(array[id], id == array.size() - 1);
        }
    }

    void ArrayInitializeExpression::writeCurrentInfo(std::ostream &ostream) {
        Expression::writeCurrentInfo(ostream);
    }

    std::size_t ArrayInitializeExpression::start() {
        return lBraceIndex;
    }

    std::size_t ArrayInitializeExpression::end() {
        if (rBraceIndex != SyntaxNode::invalidTokenIndex)
            return rBraceIndex;

        return array.back()->end();
    }

    SyntaxType ArrayInitializeExpression::getType() {
        return SyntaxType::ArrayInitializeExpression;
    }

    bool ArrayInitializeExpression::hasChild() {
        return !array.empty();
    }

    void ArrayInitializeExpression::add(const std::shared_ptr<Expression> &child) {
        array.push_back(child);
    }

    void ArrayInitializeExpression::genCode(compiler::CodeGenerator &gen, ast::Var *temp) {
        //转换成一系列的Store指令，temp就是数组名
        for(size_t i=0;i<array.size();i++){
            //temp[i] = t;
            Var* t = array[i]->getVar(gen);
            Var* offset = gen.getConstVar(i);
            gen.genStore(t,temp,offset);
        }
    }
}