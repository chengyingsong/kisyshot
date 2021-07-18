#include <ast/token.h>
namespace kisyshot::ast{


    /**
     * A global const map which stores the relation of all the types of TOKEN and their spells.
     */
    const std::unordered_map<std::string, TokenType> m_raw_token = {
#define PUNCTUATOR(X,Y) {Y, kisyshot::ast::TokenType::X},
#define KEYWORD(X) {#X, kisyshot::ast::TokenType::kw_ ## X},
#define OPERATOR(X,Y) {Y, kisyshot::ast::TokenType::op_ ## X},
#define TOKEN(X) {#X, kisyshot::ast::TokenType::X},
#include "ast/tokens.inc"
    };


    /**
     * A global const map which stores the relation of all the types of TOKEN and their spells.
     */
    const std::unordered_map<TokenType, std::string> m_token_spell = {
#define PUNCTUATOR(X, Y) {kisyshot::ast::TokenType::X, Y},
#define KEYWORD(X) {kisyshot::ast::TokenType::kw_ ## X, #X},
#define OPERATOR(X, Y) {kisyshot::ast::TokenType::op_ ## X, Y},
#define TOKEN(X) {kisyshot::ast::TokenType::X, #X},
#include "ast/tokens.inc"
    };

    /**
     * A global const map which stores the relation of all the types of PUNCTUATOR and their spells.
     */
    const std::unordered_set<TokenType> s_punctuator = {
#define PUNCTUATOR(X,Y) kisyshot::ast::TokenType::X,
#include "ast/tokens.inc"
    };

    /**
     * A global const map which stores the relation of all the types of KEYWORD and their spells.
     */
    const std::unordered_set<TokenType> s_keywords = {
#define KEYWORD(X) kisyshot::ast::TokenType::kw_ ## X,
#include "ast/tokens.inc"
    };

    /**
     * A global const map which stores the relation of all the types of OPERATOR and their spells.
     */
    const std::unordered_set<TokenType> s_operators = {
#define OPERATOR(X,Y) kisyshot::ast::TokenType::op_ ## X,
#include "ast/tokens.inc"
    };

    const std::unordered_set<char> s_operator_heads = {
#define OPERATOR(X,Y) Y[0],
#include "ast/tokens.inc"
    };
}
bool kisyshot::ast::Token::is(kisyshot::ast::TokenType type) const {
    return sameType(token_type, type);
}
