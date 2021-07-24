#include <ast/syntax/syntax_node.h>

namespace kisyshot::ast::syntax {
    std::ostream &operator<<(std::ostream &s, SyntaxType type) {
        switch (type) {
            case SyntaxType::SyntaxUnit:
                s << "SyntaxUnit";
                break;
            case SyntaxType::VariableDeclaration:
                s << "VariableDeclaration";
                break;
            case SyntaxType::VariableDefinition:
                s << "VariableDefinition";
                break;
            case SyntaxType::BlockStatement:
                s << "BlockStatement";
                break;
            case SyntaxType::IfStatement:
                s << "IfStatement";
                break;
            case SyntaxType::WhileStatement:
                s << "WhileStatement";
                break;
            case SyntaxType::NopStatement:
                s << "NopStatement";
                break;
            case SyntaxType::BreakStatement:
                s << "BreakStatement";
                break;
            case SyntaxType::ContinueStatement:
                s << "ContinueStatement";
                break;
            case SyntaxType::ReturnStatement:
                s << "ReturnStatement";
                break;
            case SyntaxType::Function:
                s << "Function";
                break;
            case SyntaxType::ParamDeclaration:
                s << "ParamDeclaration";
                break;
            case SyntaxType::ParamList:
                s << "ParamList";
                break;
            case SyntaxType::Identifier:
                s << "Identifier";
                break;
            case SyntaxType::Type:
                s << "Type";
                break;
            case SyntaxType::ArrayInitializeExpression:
                s << "ArrayInitializeExpression";
            case SyntaxType::BinaryExpression:
                s << "BinaryExpression";
                break;
            case SyntaxType::UnaryExpression:
                s << "UnaryExpression";
                break;
            case SyntaxType::IdentifierExpression:
                s << "IdentifierExpression";
                break;
            case SyntaxType::NumericLiteralExpression:
                s << "NumericLiteralExpression";
                break;
            case SyntaxType::CallExpression:
                s << "CallExpression";
                break;
            case SyntaxType::IndexExpression:
                s << "IndexExpression";
                break;
            case SyntaxType::ParenthesesExpression:
                s << "ParenthesesExpression";
                break;
            case SyntaxType::InlineComment:
                s << "InlineComment";
                break;
            case SyntaxType::InterlineComment:
                s << "InterlineComment";
                break;
            default:
                s << "Unknown";
                break;
        }
        return s;
    }

}