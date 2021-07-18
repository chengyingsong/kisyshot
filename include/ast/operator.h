#pragma once

#include "token.h"

namespace kisyshot::ast {
    enum class OperatorPrecedence :size_t {
        // left-combined:
        primary = 1,
        unary,
        multiplicative,
        addition,
        shift,
        relational,
        equality,
        logical_and,
        logical_xor,
        logical_or,
        conditional_and,
        conditional_or,
        // right-combined:
        conditional,
        assignment,
        // initial
        initial
    };

    inline bool isUnaryOperator(TokenType type) {
        switch (type){
            case TokenType::op_plus:
            case TokenType::op_minus:
            case TokenType::op_exclaim:
                return true;
            default:
                return false;
        }
    }

    inline bool isPrimaryOperator(TokenType type) {
        switch (type){
            case TokenType::l_paren:
            case TokenType::l_square:
                return true;
            default:
                return false;
        }
    }

    inline OperatorPrecedence getBinaryOperatorPrecedence(TokenType type) {
        switch (type){
                // Multiple
            case TokenType::op_multi:
            case TokenType::op_slash:
            case TokenType::op_modulus:
                return OperatorPrecedence::multiplicative;
                // Add
            case TokenType::op_plus:
            case TokenType::op_minus:
                return OperatorPrecedence::addition;
                return OperatorPrecedence::shift;
                // Relational
            case TokenType::op_greater:
            case TokenType::op_less:
            case TokenType::op_greatereq:
            case TokenType::op_lesseq:
                return OperatorPrecedence::relational;
                // Equality
            case TokenType::op_equaleq:
            case TokenType::op_exclaimeq:
                return OperatorPrecedence::equality;
                // Conditional AND
            case TokenType::op_ampamp:
                return OperatorPrecedence::conditional_and;
                // Conditional OR
            case TokenType::op_pipepipe:
                return OperatorPrecedence::conditional_or;
                // Assignment
            case TokenType::op_eq:
                return OperatorPrecedence::assignment;
            default:
                return OperatorPrecedence::initial;
        }
    }
}