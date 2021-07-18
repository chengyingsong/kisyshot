#include <compiler/lexer.h>

using namespace kisyshot::ast;
using namespace kisyshot::compiler;

namespace kisyshot::compiler {
    Lexer::Lexer(const std::shared_ptr<Context> &context) {
        _context = context;
        _code = context->code;
        _position = 0;
        _eof = false;
    }

    bool Lexer::lex() {
        bool successful = true;
        do {
            if (!next())
                successful = false;
        } while (!_eof);
        return successful;
    }

    bool Lexer::next() {
        lex_start:

        if (_eof)
            return true;

        // returns a eof Token when Lexer reaches the end of the code string_view
        if (_position >= _code.size()) {
            auto token = std::make_unique<Token>();
            token->token_type = TokenType::eof;
            token->offset = _position;
            token->raw_code = _code.substr(_position, 1);
            _context->tokens.push_back(std::move(token));
            _eof = true;
            return true;
        }

        // skip space characters
        if (std::isspace(_code[_position])) {
            _position++;
            // goto start to lex start from next Token to skip spaces
            goto lex_start;
        }

        // lex number const
        if (('0' <= _code[_position]) && (_code[_position] <= '9')) {
            return nextNumericLiteral();
        }

        // lex comments
        if (_code[_position] == '/' && (_position + 1 < _code.size())) {
            switch (_code[_position + 1]) {
                // inline comment
                case '/':
                    return nextInlineComment();
                    // interline comment
                case '*':
                    return nextInterlineComment();
                default:
                    break;
            }

        }

        // lex punctuator
        if (currTokenIs(TokenType::punctuator)) {
            auto token = std::make_unique<Token>();
            token->token_type = currTokenType();
            token->raw_code = _code.substr(_position, 1);
            token->offset = _position;
            _context->tokens.push_back(std::move(token));
            _position++;
            return true;
        }

        // lex operator
        if (s_operator_heads.count(_code[_position]) == 1) {
            return nextOperator();
        }

        // lex normal identifier
        if ((_code[_position] == '_') ||
            (('A' <= _code[_position]) && (_code[_position] <= 'Z')) ||
            (('a' <= _code[_position]) && (_code[_position] <= 'z')))
            return nextIdentifier();

        // neither of the situation is satisfied return error

        auto token = std::make_unique<Token>();
        token->token_type = TokenType::unknown;
        token->offset = _position;
        while (!isSplitter())
            _position++;
        token->raw_code = _code.substr(_position, _position - token->offset - 1);
        _context->tokens.push_back(std::move(token));
        // TODO: push error
        return false;
    }

    TokenType Lexer::currTokenType() {
        return rawTokenType(std::string{_code[_position]});
    }

    bool Lexer::currTokenIs(TokenType token_type) {
        return sameType(currTokenType(), token_type);
    }

    bool Lexer::nextNumericLiteral() {
        auto token = std::make_unique<Token>();
        token->token_type = TokenType::numeric_literal;
        token->offset = _position;

        // there are few types of numeric Token
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
                    case '8':
                    case '9':
                        // TODO: push invalid octal digit 8/9 error;
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
        // real numeric Token lex DFA
        switch (_code[_position]) {
            // we met a period Token
            case '.': {
                // if it's a normal number
                if (type == NORMAL) {
                    // set it's a float
                    type = FLOAT;
                    _position++;
                    goto continue_lex;
                }
                // number Token ended
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
                // end the number Token when met other unexpected chars
                goto form_result;
        }
        form_result:
        token->raw_code = _code.substr(startPos, _position - startPos);
        _context->tokens.push_back(std::move(token));
        return true;
    }

    bool Lexer::nextIdentifier(size_t offset) {
        if (_code.size() > _position + offset) {
            // normal identifier
            if ((_code[_position + offset] == '_') ||
                (('A' <= _code[_position + offset]) && (_code[_position + offset] <= 'Z')) ||
                (('a' <= _code[_position + offset]) && (_code[_position + offset] <= 'z')) ||
                (('0' <= _code[_position + offset]) && (_code[_position + offset] <= '9')))
                return nextIdentifier(offset + 1);
        }
        auto token = std::make_unique<Token>();
        token->offset = _position;
        token->raw_code = _code.substr(_position, offset);

        // get if the identifier have already been a keyword of yuuki language

        TokenType type = rawTokenType(std::string(token->raw_code));
        token->token_type = type == TokenType::unknown ? TokenType::identifier : type;
        _position += offset;
        _context->tokens.push_back(std::move(token));
        return true;
    }

    bool Lexer::nextOperator() {
        size_t len = 1;
        if (sameType(rawTokenType(std::string(_code.substr(_position, 2))), TokenType::operators)) {
            len = 2;
        }
        auto token = std::make_unique<Token>();
        token->offset = _position;
        token->raw_code = _code.substr(_position, len);
        token->token_type = rawTokenType(std::string(_code.substr(_position, len)));
        _context->tokens.push_back(std::move(token));
        _position += len;
        return true;
    }

    bool Lexer::nextInlineComment() {
        // handle with inline comments
        size_t startPos = _position;

        auto token = std::make_unique<Token>();
        token->token_type = TokenType::inline_comment;
        token->offset = startPos;
        // when we haven't met line switch before end of file
        while (_position < _code.size()) {
            _position++;
            // ends the Token with a new-line symbol
            if (_code[_position] == '\n') {
                break;
            }
        }
        // form the Token and return
        token->raw_code = _code.substr(startPos, _position - startPos);
        _context->tokens.push_back(std::move(token));
        return true;
    }

    bool Lexer::nextInterlineComment() {
        size_t startPos = _position;
        auto token = std::make_unique<Token>();
        token->token_type = TokenType::interline_comment;
        token->offset = startPos;
        _position += 2;
        // when we haven't met the char before the last char
        while (_position < _code.size() - 1) {
            // check if it satisfies the "*/" which is the end of interline comment
            if ((_code[_position] == '*') && (_code[_position + 1] == '/')) {
                // form the result and return
                token->raw_code = _code.substr(startPos, _position + 2 - startPos);
                _context->tokens.push_back(std::move(token));
                _position += 2;
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

    bool Lexer::isSplitter() {
        if (_code[_position] > 127)
            return false;
        switch (_code[_position]) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':
            case '_':
                return false;
        }
        return true;
    }
}