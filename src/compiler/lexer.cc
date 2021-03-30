#include "compiler/lexer.h"

#include "ast/token.h"
using namespace kisyshot::ast;
using namespace kisyshot::compiler;
lexer::lexer(const std::shared_ptr<context> &context) {
    _context = context;
    _code = context->code;
    _position = 0;
    _eof = false;
}

bool lexer::lex() {
    bool successful = true;
    do {
        if (!next())
            successful = false;
    } while (!_eof);
    return successful;
}

bool lexer::next() {
    lex_start:

    if (_eof)
        return true;

    // returns a eof token when lexer reaches the end of the code string_view
    if (_position >= _code.size()) {
        auto token = std::make_unique<class token>();
        token->token_type = token_type_t::eof;
        token->offset = _position;
        token->raw_code = _code.substr(_position, 1);
        _context->tokens.push_back(std::move(token));
        _eof = true;
        return true;
    }

    // skip space characters
    if (std::isspace(_code[_position])) {
        _position++;
        // goto start to lex start from next token to skip spaces
        goto lex_start;
    }

    // lex number const
    if (('0' <= _code[_position]) && (_code[_position] <= '9')) {
        return next_numeric_literal();
    }

    // lex comments
    if (_code[_position] == '/' && (_position + 1 < _code.size())) {
        switch (_code[_position + 1]) {
            // inline comment
            case '/':
                return next_inline_comment();
                // interline comment
            case '*':
                return next_interline_comment();
            default:
                break;
        }

    }

    // lex punctuator
    if (curr_token_is(token_type_t::punctuator)){
        auto token = std::make_unique<class token>();
        token->token_type = curr_token_type();
        token->raw_code = _code.substr(_position, 1);
        token->offset = _position;
        _context->tokens.push_back(std::move(token));
        _position++;
        return true;
    }

    // lex operator
    if (s_operator_heads.count(_code[_position]) == 1) {
        return next_operator();
    }

    // lex normal identifier
    if ((_code[_position] == '_') ||
        (('A' <= _code[_position]) && (_code[_position] <= 'Z')) ||
        (('a' <= _code[_position]) && (_code[_position] <= 'z')))
        return next_identifier();

    // neither of the situation is satisfied return error

    auto token = std::make_unique<class token>();
    token->token_type = token_type_t::unknown;
    token->offset = _position;
    while (!is_splitter())
        _position++;
    token->raw_code = _code.substr(_position, _position - token->offset - 1);
    _context->tokens.push_back(std::move(token));
    // TODO: push error
    return false;
}

token_type_t lexer::curr_token_type() {
    return raw_token_type(std::string{_code[_position]});
}

bool lexer::curr_token_is(token_type_t token_type) {
    return same_type(curr_token_type(), token_type);
}

bool lexer::next_numeric_literal() {
    auto token = std::make_unique<class token>();
    token->token_type = token_type_t::numeric_literal;
    token->offset = _position;

    // there are few types of numeric token
    // we can do some basic valid check for it
    enum NumericType {
        NORMAL,
        FLOAT,
        SCIENTIFIC,
        OCT,
        HEX
    };
    // default type is NORMAL
    NumericType type = NORMAL;
    size_t startPos = _position;
    // initial judgements:
    // numbers start with 0
    if (_code[_position] == '0') {
        if (_position + 1 < _code.size()) {
            switch (_code[_position + 1]) {
                case '.':
                    // for float numbers like '0.123'
                    type = FLOAT;
                    _position += 2;
                    break;
                case 'x':
                case 'X':
                    // for hex numbers like '0x88ff'
                    type = HEX;
                    _position += 2;
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                    // for octane numbers like '012'
                    type = OCT;
                    _position += 1;
                    break;
                default:
                    // just a single zero literal
                    token->raw_code = _code.substr(_position, 1);
                    _context->tokens.push_back(std::move(token));
                    _position++;
                    return true;
            }
        }
    }
    continue_lex:
    // real numeric token lex DFA
    switch (_code[_position]) {
        // we met a period token
        case '.': {
            // if it's a normal number
            if (type == NORMAL) {
                // set it's a float
                type = FLOAT;
                _position++;
                goto continue_lex;
            }
            // number token ended
            goto form_result;
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7': {
            _position++;
            goto continue_lex;
        }
        case '8':
        case '9': {
            // we should handle special situations for octane numbers
            if (type == OCT) {
                // '8' and '9' should be treated as an end sign of a octane number
                goto form_result;
            }
            _position++;
            goto continue_lex;
        }
        case 'a':
        case 'A':
        case 'b':
        case 'B':
        case 'c':
        case 'C':
        case 'd':
        case 'D':
        case 'f':
        case 'F': {
            // we should handle special situations for hex numbers
            // these letters are treated as numbers only in hex mode
            if (type == HEX) {
                _position++;
                goto continue_lex;
            }
            // we should just end the lex of a number when we met a
            // letter that afters a non-hex number
            goto form_result;
        }
        case 'e':
        case 'E': {
            // scientific write mode of a number
            // like "123e-7" which means 123 * math.pow(10,-7)
            if (type == FLOAT || type == NORMAL) {
                if ((_position + 1 < _code.size()) &&
                    (((_code[_position + 1] == '+')) || ((_code[_position + 1] == '-'))))
                    _position++;
                _position++;
                type = SCIENTIFIC;
                goto continue_lex;
            } else if (type == HEX) {
                // 'e' and 'E' are treated as number members in hex mode
                _position++;
                goto continue_lex;
            } else {
                // we should just end the lex of a number when we met a
                // letter that afters a non-hex number
                goto form_result;
            }
        }
        default:
            // end the number token when met other unexpected chars
            goto form_result;
    }
    form_result:
    token->raw_code = _code.substr(startPos, _position - startPos);
    _context->tokens.push_back(std::move(token));
    return true;
}

bool lexer::next_identifier(size_t offset) {
    if (_code.size() > _position + offset) {
        // normal identifier
        if ((_code[_position + offset] == '_') ||
            (('A' <= _code[_position + offset]) && (_code[_position + offset] <= 'Z')) ||
            (('a' <= _code[_position + offset]) && (_code[_position + offset] <= 'z')) ||
            (('0' <= _code[_position + offset]) && (_code[_position + offset] <= '9')))
            return next_identifier(offset + 1);
    }
    auto token = std::make_unique<class token>();
    token->offset = _position;
    token->raw_code = _code.substr(_position, offset);

    // get if the identifier have already been a keyword of yuuki language

    token_type_t type = raw_token_type(std::string(token->raw_code));
    token->token_type = type == token_type_t::unknown ? token_type_t::identifier : type;
    _position += offset;
    _context->tokens.push_back(std::move(token));
    return true;
}

bool lexer::next_operator() {
    size_t len = 1;
    if (same_type(raw_token_type(std::string(_code.substr(_position, 2))), token_type_t::operators)){
        len = 2;
    }
    auto token = std::make_unique<class token>();
    token->offset = _position;
    token->raw_code = _code.substr(_position, len);
    token->token_type = raw_token_type(std::string(_code.substr(_position, len)));
    _context->tokens.push_back(std::move(token));
    _position += len;
    return true;
}

bool lexer::next_inline_comment() {
    // handle with inline comments
    size_t startPos = _position;

    auto token = std::make_unique<class token>();
    token->token_type = token_type_t::inline_comment;
    token->offset = startPos;
    // when we haven't met line switch before end of file
    while (_position < _code.size()) {
        _position++;
        // ends the token with a new-line symbol
        if (_code[_position] == '\n') {
            break;
        }
    }
    // form the token and return
    token->raw_code = _code.substr(startPos, _position - startPos);
    _context->tokens.push_back(std::move(token));
    return true;
}

bool lexer::next_interline_comment() {
    size_t startPos = _position;
    auto token = std::make_unique<class token>();
    token->token_type = token_type_t::interline_comment;
    token->offset = startPos;
    _position += 2;
    // when we haven't met the char before the last char
    while (_position < _code.size() - 1) {
        // check if it satisfies the "*/" which is the end of interline comment
        if ((_code[_position] == '*') && (_code[_position + 1] == '/')) {
            // form the result and return
            token->raw_code = _code.substr(startPos, _position + 2 - startPos);
            _context->tokens.push_back(std::move(token));
            _position+=2;
            return true;
        }
        _position++;
    }
    // we are going to met the eof but we haven't met the end of the symbol to end the interline comment
    // move to the end contain the last char
    _position += 1;
    // form the result and push the error to the diagnostic
    token->raw_code = _code.substr(startPos, _position - startPos);
    _context->tokens.push_back(std::move(token));
    // TODO: push error
    return false;
}

bool lexer::is_splitter() {
    if(_code[_position]>127)
        return false;
    switch (_code[_position]){
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
        case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
        case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': case 'A': case 'B': case 'C': case 'D':
        case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
        case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
        case 'Y': case 'Z': case '_':
            return false;
    }
    return true;
}


