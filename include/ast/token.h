#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
namespace kisyshot::ast {
    /**
     * A enum type for defining types of token, by using hack of marcos to generate codes.
     */
    enum class token_type_t: uint8_t {
#define TOKEN(X) X,
#include "tokens.inc"
        punctuator ,
        operators  ,
        keywords   ,
        undefined  = 255
    };

    enum class operator_type_t: uint8_t {
#define OPERATOR(X, Y) op_ ## X = (uint8_t)token_type_t::op_ ## X,
#include "tokens.inc"
    };

    /**
     * A global const map which stores the relation of all the types of TOKEN and their spells.
     */
    const std::unordered_map<std::string,token_type_t> m_raw_token = { /* NOLINT */
#define PUNCTUATOR(X,Y) {Y,token_type_t::X},
#define KEYWORD(X) {#X,token_type_t::kw_ ## X},
#define OPERATOR(X,Y) {Y,token_type_t::op_ ## X},
#define MODIFIER(X) {#X,token_type_t::kw_ ## X},
#include "tokens.inc"
    };

    /**
     * A global const map which stores the relation of all the types of PUNCTUATOR and their spells.
     */
    const std::unordered_set<token_type_t> s_punctuator = { /* NOLINT */
#define PUNCTUATOR(X,Y) token_type_t::X,
#include "tokens.inc"
    };

    /**
     * A global const map which stores the relation of all the types of KEYWORD and their spells.
     */
    const std::unordered_set<token_type_t> s_keywords = { /* NOLINT */
#define KEYWORD(X) token_type_t::kw_ ## X,
#include "tokens.inc"
    };

    /**
     * A global const map which stores the relation of all the types of OPERATOR and their spells.
     */
    const std::unordered_set<token_type_t> s_operators = { /* NOLINT */
#define OPERATOR(X,Y) token_type_t::op_ ## X,
#include "tokens.inc"
    };

    const std::unordered_set<char> s_operator_heads = { /* NOLINT */
#define OPERATOR(X,Y) Y[0],
#include "tokens.inc"
    };


    struct token {
        /**
         * The type of the token.
         */
        token_type_t     token_type = token_type_t::undefined;
        /**
         * The relative position, compared to the start of the code.
         */
        std::size_t      offset = 0;
        /**
         * The raw code string_view which spilt from code by the lexer.
         */
        std::string_view raw_code;
        /**
         * Judge if the token matches the given type.
         * @param type: the type to be compared
         * @return true when type matches and false in other situations.
         */
        [[nodiscard]] bool is(token_type_t type) const;
    };

    inline bool same_type(token_type_t lhs, token_type_t rhs){
        switch (rhs) {
            case token_type_t::punctuator:
                return s_punctuator.count(lhs) == 1;
            case token_type_t::keywords:
                return s_keywords.count(lhs) == 1;
            case token_type_t::operators:
                return s_operators.count(lhs) == 1;
            default:
                return lhs == rhs;
        }
    }

    inline token_type_t raw_token_type(const std::string &raw){
        if (m_raw_token.count(raw) == 0)
            return token_type_t::unknown;
        return m_raw_token.at(raw);
    }
}
