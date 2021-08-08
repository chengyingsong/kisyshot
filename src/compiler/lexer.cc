#include <compiler/lexer.h>
#include <diagnostic/diagnostic.h>
#include <unordered_set>
using namespace kisyshot::ast;
using namespace kisyshot::compiler;

namespace kisyshot::compiler {
    std::unordered_set<int> spaces{0x1680, 0x180E, 0x2000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006, 0x2007,
                                   0x2008, 0x2009, 0x200A, 0x200B, 0x202F, 0x205F, 0x3000, 0xFEFF};

    Lexer::Lexer(const std::shared_ptr<Context> &context,
                 const std::shared_ptr<diagnostic::DiagnosticStream> &diagnosticStream) {
        _context = context;
        _code = context->code;
        _diagnosticStream = diagnosticStream;
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

        if ((unsigned char)_code[_position] == 0xA0){
            _position++;
            goto lex_start;
        }

        // skip utf-8 special spaces
        if (_position + 1 < _code.size()) {
            unsigned char high = _code[_position], low = _code[_position + 1];
            int r = (high << 8) + low;
            if (high == 0x20 || high == 0x30 ||high == 0xFE){
                if (spaces.count(r) == 1){
                    _position += 2;
                    goto lex_start;
                }
            }
        }

        // lex number const
        if (('0' <= _code[_position]) && (_code[_position] <= '9')) {
            return nextNumericLiteral();
        }

        // lex string const
        if (_code[_position] == '\"') {
            return nextStringLiteral();
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
        do {
            _position++;
        } while (!isSplitter());
        token->raw_code = _code.substr(token->offset, _position - token->offset);
        _diagnosticStream << diagnostic::Diagnostic(diagnostic::Error, _context, "invalid chars")
                .emphasize(std::string(token->raw_code))
                .at(_context->tokens.size());
        _context->tokens.push_back(std::move(token));
        return false;
    }

    ast::TokenType Lexer::currTokenType() {
        return rawTokenType(std::string{_code[_position]});
    }

    bool Lexer::currTokenIs(ast::TokenType token_type) {
        return sameType(currTokenType(), token_type);
    }

    bool Lexer::nextStringLiteral() {
        auto token = std::make_unique<Token>();
        token->offset = _position;
        token->token_type = TokenType::string_literal;
        size_t startPos = _position;

        bool escaped = false;
        // skip the '"'
        _position++;
        // before we met the end of the file
        while (_position < _code.size()) {
            // get the current char, handle it with a DFA
            switch (_code[_position]) {
                // we met a new '"'
                case '\"': {
                    // if the quote mark is not escaped, marks the string ended
                    if (!escaped) {
                        // form a token info and return
                        token->raw_code = _code.substr(startPos + 1, _position - startPos - 1);
                        _context->tokens.push_back(std::move(token));
                        // skip the close '"'
                        _position++;
                        return true;
                    }
                    // if th quote mark is escaped, we treat it like other chars and continue
                    escaped = false;
                    break;
                }
                    // we met the escape mark '\'
                case '\\': {
                    // mark the escape mode negative to itself
                    // true: when the current char escaped, it means current '\' is a escaped char so there
                    //       have already a char escaped, we should set it to false;
                    // false:when the current char not escaped, it means current '\' should be treated as a
                    //       mark of next char escaping, we should set it to true.
                    escaped = !escaped;
                    break;
                }
                    // we met the new-line code
                case '\n': {
                    // when the new-line symbol is not escaped
                    if (!escaped) {
                        // it means it's a end of string that didn't end
                        // form the result and push an error
                        token->raw_code = _code.substr(startPos, 1);
                        _context->tokens.push_back(std::move(token));
                        _position++;
                        _diagnosticStream << diagnostic::Diagnostic(diagnostic::Error, _context, "string not closed")
                                .at(_context->tokens.size() - 1);
                        return false;
                    }
                    // when the new-line symbol is escaped, set escaped variable to false
                    escaped = false;
                    break;
                }
                default: {
                    escaped = false;
                    break;
                }
            }
            _position++;
        }
        // we met the end of the code, but the mark ends the string still not found
        // instantly form the token result and push an error to the diagnostic
        token->raw_code = _code.substr(startPos, 1);
        _context->tokens.push_back(std::move(token));
        _diagnosticStream << diagnostic::Diagnostic(diagnostic::Error, _context, "string not closed")
                .at(_context->tokens.size() - 1);
        return false;
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
                        while (!isSplitter())
                            _position++;

                        token->raw_code = _code.substr(startPos, _position - startPos);
                        token->token_type = ast::TokenType::numeric_literal;
                        _context->tokens.push_back(std::move(token));
                        _diagnosticStream << diagnostic::Diagnostic(diagnostic::Error, _context,
                                                                    "octal numeric const should not contain '8' or '9'")
                                .at(_context->tokens.size() - 1);
                        return false;
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

        ast::TokenType type = rawTokenType(std::string(token->raw_code));
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
        token->raw_code = _code.substr(startPos, 2);
        _context->tokens.push_back(std::move(token));
        _diagnosticStream << diagnostic::Diagnostic(diagnostic::Error, _context,
                                                    "unterminated comment").at(
                _context->tokens.size() - 1);
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