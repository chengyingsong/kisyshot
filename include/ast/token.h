#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace kisyshot::ast {
    /**
     * A enum type for defining types of Token, by using hack of marcos to generate codes.
     */
    enum class TokenType: uint8_t {
#define TOKEN(X) X,
#include "tokens.inc"
        punctuator ,
        operators  ,
        keywords   ,
        comments   ,
        undefined  = 255
    };


    /**
     * A global const map which stores the relation of all the types of TOKEN and their spells.
     */
    const std::unordered_map<std::string,TokenType> m_raw_token = { /* NOLINT */
#define PUNCTUATOR(X,Y) {Y,TokenType::X},
#define KEYWORD(X) {#X,TokenType::kw_ ## X},
#define OPERATOR(X,Y) {Y,TokenType::op_ ## X},
#include "tokens.inc"
    };

    /**
     * A global const map which stores the relation of all the types of TOKEN and their spells.
     */
    const std::unordered_map<TokenType, std::string> m_token_spell = { /* NOLINT */
#define PUNCTUATOR(X, Y) {TokenType::X, Y},
#define KEYWORD(X) {TokenType::kw_ ## X, #X},
#define OPERATOR(X, Y) {TokenType::op_ ## X, Y},
#include "tokens.inc"
    };

    /**
     * A global const map which stores the relation of all the types of PUNCTUATOR and their spells.
     */
    const std::unordered_set<TokenType> s_punctuator = { /* NOLINT */
#define PUNCTUATOR(X,Y) TokenType::X,
#include "tokens.inc"
    };

    /**
     * A global const map which stores the relation of all the types of KEYWORD and their spells.
     */
    const std::unordered_set<TokenType> s_keywords = { /* NOLINT */
#define KEYWORD(X) TokenType::kw_ ## X,
#include "tokens.inc"
    };

    /**
     * A global const map which stores the relation of all the types of OPERATOR and their spells.
     */
    const std::unordered_set<TokenType> s_operators = { /* NOLINT */
#define OPERATOR(X,Y) TokenType::op_ ## X,
#include "tokens.inc"
    };

    const std::unordered_set<char> s_operator_heads = { /* NOLINT */
#define OPERATOR(X,Y) Y[0],
#include "tokens.inc"
    };


    struct Token {
        /**
         * The type of the Token.
         */
        TokenType token_type = TokenType::undefined;
        /**
         * The relative position, compared to the start of the code.
         */
        std::size_t offset = 0;
        /**
         * The raw code string_view which spilt from code by the Lexer.
         */
        std::string_view raw_code;
        /**
         * Judge if the Token matches the given type.
         * @param type: the type to be compared
         * @return true when type matches and false in other situations.
         */
        [[nodiscard]] bool is(TokenType type) const;
    };

    inline bool sameType(TokenType lhs, TokenType rhs){
        switch (rhs) {
            case TokenType::punctuator:
                return s_punctuator.count(lhs) == 1;
            case TokenType::keywords:
                return s_keywords.count(lhs) == 1;
            case TokenType::operators:
                return s_operators.count(lhs) == 1;
            case TokenType::comments:
                return lhs == TokenType::inline_comment || lhs == TokenType::interline_comment;
            default:
                return lhs == rhs;
        }
    }

    inline TokenType rawTokenType(const std::string &raw){
        if (m_raw_token.count(raw) == 0)
            return TokenType::unknown;
        return m_raw_token.at(raw);
    }

    inline std::string getTokenSpell(TokenType token){
        return m_token_spell.at(token);
    }
}
