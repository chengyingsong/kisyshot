#include "ast/token.h"

bool kisyshot::ast::token::is(kisyshot::ast::token_type_t type) const {
    return same_type(token_type, type);
}
