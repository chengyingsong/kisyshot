#include <compiler/parser.h>

using namespace kisyshot::ast;
using namespace kisyshot::ast::syntax;
namespace kisyshot::compiler {
    Parser::Parser(const std::shared_ptr<Context> &context,
                   const std::shared_ptr<diagnostics::DiagnosticStream> &diagStream) {
        _context = context;
        _diagnosticStream = diagStream;
        _current = 0;
        _lookahead = 0;
        recoverPosition();
    }

    void Parser::parse() {
        _context->syntaxTree = std::make_shared<SyntaxUnit>();
        while (_current < _context->tokens.size() - 1) {
            switch (current()) {
                case ast::TokenType::kw_const: {
                    _context->syntaxTree->add(parseVariableDeclaration());
                    break;
                }
                case ast::TokenType::identifier: {
                    size_t start = _current;
                    step();
                    if (lookahead() == ast::TokenType::l_paren) {
                        _current = start;
                        recoverPosition();
                        _context->syntaxTree->add(parseFunction());
                    } else {
                        _current = start;
                        recoverPosition();
                        _context->syntaxTree->add(parseVariableDeclaration());

                    }
                    break;
                }
                default: {
                    // error recoverPosition
                    // we choose to recoverPosition to some status like:
                    //    [const] type id [ = E][, ...];
                    // or type id(...){...}
                    recover_next:
                    step();
                    switch (current()) {
                        case ast::TokenType::kw_const:
                            // we met const var decl, try recoverPosition
                            break;
                        case ast::TokenType::identifier: {
                            if (lookahead() == ast::TokenType::identifier) {
                                move(_lookahead);
                                switch (lookahead()) {
                                    case ast::TokenType::op_eq:
                                    case ast::TokenType::semi:
                                    case ast::TokenType::l_paren:
                                    case ast::TokenType::comma:
                                        break;
                                    default:
                                        goto recover_next;
                                }
                            }
                        }
                        case ast::TokenType::eof:
                            return;
                        default:
                            goto recover_next;
                    }
                    recoverPosition();
                    break;
                }
            }
        }
    }

    std::shared_ptr<ast::syntax::Identifier> Parser::parseIdentifier() {
        if (current() == ast::TokenType::identifier) {
            auto id = std::make_shared<Identifier>();
            id->identifier = (std::string) _context->tokens[_current]->raw_code;
            id->tokenIndex = _current;
            step();
            return id;
        }
        return nullptr;
    }

    std::shared_ptr<ast::syntax::Function> Parser::parseFunction() {

        auto function = std::make_shared<Function>();
        function->returnType = parseType();
        function->name = parseIdentifier();
        function->params = std::make_shared<ParamList>();
        // size_t lParen = _current;
        step();
        while (current() == ast::TokenType::identifier) {
            auto para = std::make_shared<ParamDeclaration>();
            para->type = parseType();
            para->name = parseIdentifier();
            size_t dim = 0;
            while (current() == ast::TokenType::l_square) {
                while (current() != ast::TokenType::r_square && current() != ast::TokenType::r_paren)
                    step();
                dim++;
                step();
            }
            para->dimension = dim;
            function->params->add(para);
            if (current() == ast::TokenType::comma)
                step();
        }
        if (current() != ast::TokenType::r_paren) {
            // TODO
        } else {
            function->rParenIndex = _current;
            step();
        }
        if (current() != ast::TokenType::l_brace) {
            // TODO
        } else {
            function->body = parseBlockStatement();
        }
        return function;
    }

    std::shared_ptr<ast::syntax::Expression>
    Parser::parseExpression(const std::set<ast::TokenType> &endTokens, OperatorPrecedence parentPrecedence) {
        recoverPosition();
        std::shared_ptr<Expression> left;
        if (isUnaryOperator(current()) && OperatorPrecedence::unary <= parentPrecedence) {
            size_t opIndex = _current;
            step();
            left = parseExpression(endTokens, OperatorPrecedence::unary);
            auto unary = std::make_shared<UnaryExpression>();
            unary->operatorType = _context->tokens[opIndex]->token_type;
            unary->opIndex = opIndex;
            unary->right = left;
            left = unary;
        } else {
            switch (current()) {
                case ast::TokenType::l_paren: {
                    size_t lParenIndex = _current;
                    step();
                    left = parseExpression(endTokens);
                    size_t rParenIndex = _current;
                    if (current() != ast::TokenType::r_paren) {
                        rParenIndex = SyntaxNode::invalidTokenIndex;
                        // TODO: print ')' expected
                    } else {
                        step();
                    }
                    auto parentheses = std::make_shared<ParenthesesExpression>();
                    parentheses->leftParenIndex = lParenIndex;
                    parentheses->rightParenIndex = rParenIndex;
                    parentheses->innerExpression = left;
                    left = parentheses;
                    break;
                }
                case ast::TokenType::l_brace: {
                    auto array = std::make_shared<ArrayInitializeExpression>();
                    array->lBraceIndex = _current;
                    step();
                    if (current() == ast::TokenType::r_brace) {
                        step();
                    } else {
                        bool parseArray = true;
                        do {
                            auto elem = parseExpression(endTokens, parentPrecedence);
                            if (elem != nullptr) {
                                array->add(elem);
                            }
                            switch (current()) {
                                case ast::TokenType::comma: {
                                    if (elem == nullptr) {
                                        // TODO: expression expected
                                    }
                                    step();
                                    break;
                                }
                                case ast::TokenType::r_brace: {
                                    if (elem == nullptr) {
                                        // TODO: expression expected
                                    }
                                    parseArray = false;
                                    step();
                                    break;
                                }
                                case ast::TokenType::semi: {
                                    // TODO: r paren expected
                                    parseArray = false;
                                    break;
                                }
                                default:
                                    break;
                            }

                        } while (parseArray);
                    }
                    left = array;
                    break;
                }
                case ast::TokenType::identifier: {
                    auto identifier = std::make_shared<IdentifierExpression>();
                    identifier->name = parseIdentifier();
                    left = identifier;
                    break;
                }
                case ast::TokenType::numeric_literal: {
                    auto number = std::make_shared<NumericLiteralExpression>();
                    number->rawCode = _context->tokens[_current]->raw_code;
                    number->tokenIndex = _current;
                    left = number;
                    step();
                    break;
                }
                default: {
                    left = nullptr;
                    if (getBinaryOperatorPrecedence(current()) == OperatorPrecedence::initial) {
                        // std::size_t startIndex = _current; /* NOLINT */
                        // we have moved into a position we can handle we should just push the error happened
                        // since ErrorBuilder::error().ranges() is not implemented yet, just do nothing here
                        while (endTokens.count(current()) == 0)
                            step();
                        // TODO: push error 'UnexpectedTokens'
                        // unexpected tokens, move to endTokens;
                    }
                    break;
                }
            }
        }
        while (true) {
            // we met the end of the expression
            if (std::find(endTokens.begin(), endTokens.end(), current()) != endTokens.end())
                break;
            parsePrimary:

            switch (current()) {
                case TokenType::l_square: {
                    std::size_t lSquare = _current;
                    step();
                    auto newEndTokens = endTokens;
                    newEndTokens.insert(TokenType::r_square);
                    auto index = parseExpression(newEndTokens);
                    if (current() == TokenType::r_square) {
                        auto indexed = std::make_shared<IndexExpression>();
                        indexed->lSquareIndex = lSquare;
                        indexed->rSquareIndex = _current;
                        indexed->indexedExpr = left;
                        indexed->indexerExpr = index;

                        left = indexed;
                        if (lookahead() == TokenType::l_square) {
                            step();
                            goto parsePrimary;
                        }
                        step();
                    } else {
                        auto indexed = std::make_shared<IndexExpression>();
                        indexed->lSquareIndex = lSquare;
                        indexed->indexedExpr = left;
                        indexed->indexerExpr = index;
                        left = indexed;
                        while (endTokens.count(current()) == 0)
                            step();
                        // TODO: push ] expected
                    }
                    break;
                }
                case TokenType::l_paren: {
                    // we are going to handle trivial method calls here
                    // make a copy of end tokens
                    auto newEndTokens = endTokens;
                    if (left != nullptr && left->getType() == SyntaxType::IdentifierExpression) {
                        newEndTokens.insert(TokenType::r_paren);
                        auto callExpr = std::make_shared<CallExpression>();
                        callExpr->lParenIndex = _current;
                        callExpr->name = std::dynamic_pointer_cast<IdentifierExpression>(left)->name;
                        parseNextParam:
                        step();
                        if (current() != TokenType::r_paren) {
                            auto para = parseExpression(newEndTokens);
                            callExpr->add(para);
                            switch (current()) {
                                case TokenType::comma:
                                    goto parseNextParam;
                                case TokenType::r_paren:
                                    callExpr->rParenIndex = _current;
                                    step();
                                    break;
                                default:
                                    //TODO: push ')' expected.
                                    while (endTokens.count(current()) == 0)
                                        step();
                            }
                        } else {
                            callExpr->rParenIndex = _current;
                            step();
                        }
                        left = callExpr;
                    }
                    break;
                }
                default:
                    break;
            }
            // we are now going to handle with binary expressions
            auto precedence = getBinaryOperatorPrecedence(current());
            size_t opIndex = _current;
            if (precedence == OperatorPrecedence::initial)
                break;
            if (precedence >= parentPrecedence && precedence != OperatorPrecedence::assignment)
                break;
            step();
            auto right = parseExpression(endTokens, precedence);
            auto binary = std::make_shared<BinaryExpression>();
            binary->left = left;
            binary->operatorType = _context->tokens[opIndex]->token_type;
            binary->opIndex = opIndex;
            binary->right = right;
            left = binary;
        }
        return left;
    }

    std::shared_ptr<ast::syntax::Statement> Parser::parseStatement() {

        switch (current()) {
            case ast::TokenType::kw_return: {
                auto ret = std::make_shared<ReturnStatement>();
                ret->returnTokenIndex = _current;
                if (lookahead() == ast::TokenType::semi) {
                    ret->semiTokenIndex = _lookahead;
                    step();
                    step();
                    return ret;
                } else {
                    step();
                    ret->value = parseExpression();
                    if (current() == ast::TokenType::semi) {
                        ret->semiTokenIndex = _current;
                        step();
                    } else {
                        // TODO: semi expected
                    }
                    return ret;
                }
            }
            case ast::TokenType::kw_continue: {
                std::shared_ptr<ContinueStatement> cont = std::make_shared<ContinueStatement>();
                cont->continueTokenIndex = _current;
                if (lookahead() == ast::TokenType::semi) {
                    cont->semiTokenIndex = _lookahead;
                    step();
                } else {
                    // TODO: push ';' expected
                }
                step();
                return cont;
            }
            case ast::TokenType::kw_break: {
                std::shared_ptr<BreakStatement> brk = std::make_shared<BreakStatement>();
                brk->breakTokenIndex = _current;
                if (lookahead() == ast::TokenType::semi) {
                    brk->semiTokenIndex = _lookahead;
                    step();
                } else {
                    // TODO: push ';' expected
                }
                step();
                return brk;
            }
            case ast::TokenType::kw_if: {
                return parseIfStatement();
            }
            case ast::TokenType::kw_while: {
                return parseWhileStatement();
            }
            case ast::TokenType::kw_const: {
                return parseVariableDeclaration();
            }
            case ast::TokenType::identifier: {
                if (lookahead() == ast::TokenType::identifier) {
                    return parseVariableDeclaration();
                }
                auto stmt = std::make_shared<ExpressionStatement>();
                stmt->expression = parseExpression({TokenType::semi});
                if (current() == ast::TokenType::semi) {
                    stmt->semiTokenIndex = _current;
                    step();
                } else {
                    // TODO: semi expected
                }
                return stmt;
            }
            case ast::TokenType::l_brace: {
                return parseBlockStatement();
            }
            default: {
                // TODO: error recover
            }
        }
        return nullptr;
    }

    std::shared_ptr<ast::syntax::BlockStatement> Parser::parseBlockStatement() {
        if (current() == ast::TokenType::l_brace) {
            auto block = std::make_shared<BlockStatement>();
            block->lBraceTokenIndex = _current;
            step();
            while (_current < _context->tokens.size() && current() != ast::TokenType::r_brace) {
                auto stmt = parseStatement();
                block->add(stmt);
            }
            if (current() == ast::TokenType::r_brace)
                step();
            return block;
        }
        return nullptr;
    }

    std::shared_ptr<ast::syntax::IfStatement> Parser::parseIfStatement() {
        if (current() == ast::TokenType::kw_if && lookahead() == ast::TokenType::l_paren) {
            auto stmt = std::make_shared<IfStatement>();
            stmt->ifTokenIndex = _current;
            step();
            stmt->lParenIndex = _current;
            step();
            stmt->condition = parseExpression({TokenType::r_paren});
            stmt->rParenIndex = _current;
            step();
            stmt->ifClause = parseStatement();
            if (current() == ast::TokenType::kw_else) {
                stmt->elseTokenIndex = _current;
                step();
                stmt->elseClause = parseStatement();
            }
            return stmt;
        }
        return nullptr;
    }

    std::shared_ptr<ast::syntax::WhileStatement> Parser::parseWhileStatement() {
        if (current() == ast::TokenType::kw_while && lookahead() == ast::TokenType::l_paren) {
            auto stmt = std::make_shared<WhileStatement>();
            stmt->whileTokenIndex = _current;
            step();
            stmt->lParenIndex = _current;
            step();
            stmt->condition = parseExpression({TokenType::r_paren});
            stmt->rParenIndex = _current;
            step();
            stmt->body = parseStatement();
            return stmt;
        }
        return nullptr;
    }

    bool Parser::move(size_t &outPos) {
        while (++outPos < _context->tokens.size() &&
               sameType(_context->tokens[outPos]->token_type, ast::TokenType::comments));
        return outPos < _context->tokens.size();
    }

    ast::TokenType Parser::current() {
        return _context->tokens[_current]->token_type;
    }

    ast::TokenType Parser::lookahead() {
        return _context->tokens[_lookahead]->token_type;
    }

    bool Parser::step() {
        return move(_current) && recoverPosition();
    }

    bool Parser::recoverPosition() {
        _lookahead = _current;
        return move(_lookahead);
    }

    std::shared_ptr<ast::syntax::VarDeclaration> Parser::parseVariableDeclaration() {
        size_t constPos = SyntaxNode::invalidTokenIndex;
        std::shared_ptr<Type> type;
        std::shared_ptr<VarDeclaration> decl;
        if (current() == ast::TokenType::kw_const) {
            constPos = _current;
            step();
        }
        if (current() == ast::TokenType::identifier && lookahead() == ast::TokenType::identifier) {
            std::shared_ptr<Identifier> varName;

            decl = std::make_shared<VarDeclaration>();
            decl->type = parseType();
            decl->constTokenIndex = constPos;
            while (_current < _context->tokens.size() && current() == ast::TokenType::identifier) {
                auto def = std::make_shared<VarDefinition>();
                def->varName = parseIdentifier();
                bool varEnd = false;
                do {
                    switch (current()) {
                        case ast::TokenType::l_square: {
                            step();
                            if (current() == ast::TokenType::r_square) {
                                // TODO: push expr expected
                                break;
                            }
                            auto arrVal = parseExpression({TokenType::r_square, TokenType::semi});
                            def->add(arrVal);
                            if (current() != ast::TokenType::r_square) {
                                // TODO: push ']' expected
                            }
                            step();
                            break;
                        }
                        case ast::TokenType::op_eq: {
                            def->equalTokenIndex = _current;
                            step();
                            if (current() == ast::TokenType::semi) {
                                // TODO: push expr expected
                            }
                            def->initialValue = parseExpression({TokenType::semi, TokenType::comma});
                            break;
                        }
                        case ast::TokenType::eof:
                        case ast::TokenType::comma:
                        case ast::TokenType::semi: {
                            varEnd = true;
                            break;
                        }
                        default: {
                            // TODO: push identifier expected and try to move to ',' or ';'
                            break;
                        }
                    }
                } while (!varEnd);
                decl->add(def);
                if (current() == ast::TokenType::comma) {
                    step();
                }
            }
            if (current() != ast::TokenType::semi) {
                // TODO:
            } else {
                decl->semiTokenIndex = _current;
                step();
            }
        }

        return decl;
    }

    std::shared_ptr<ast::syntax::Type> Parser::parseType() {
        auto  type = std::make_shared<ast::syntax::Type>();
        type->typeName = parseIdentifier();
        return type;
    }
}
