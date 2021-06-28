#include <ast/token.h>

bool kisyshot::ast::Token::is(kisyshot::ast::TokenType type) const {
    return sameType(token_type, type);
}
