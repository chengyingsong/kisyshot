#include <compiler/parser.h>
#include <cassert>
using namespace kisyshot::ast;
using namespace kisyshot::ast::syntax;
namespace kisyshot::compiler {
    Parser::Parser(const std::shared_ptr<Context> &context,
                   const std::shared_ptr<diagnostic::DiagnosticStream> &diagStream) {
        _context = context;
        _diagnosticStream = diagStream;
        _current = 0;
        _lookahead = 0;
    }

    void Parser::parse() {
        recover();
        _context->syntaxTree = std::make_shared<SyntaxUnit>();
        while (_current < _context->tokens.size() - 1) {
            if (current() == ast::TokenType::kw_const) {
                // when meet a const expression
                // we assert that it's a variable declaration here
                _context->syntaxTree->add(parseVariableDeclaration());
            } else if (current() == ast::TokenType::identifier) {
                // when meet a identifier, we should look ahead to decide
                // <identifier> <identifier> (...) ... => function
                // <identifier> <identifier> , ...
                // <identifier> <identifier> [..]
                // <identifier> <identifier> = ...
                // <identifier> <identifier> ;         => variable declaration
                pushRecoverAndStep();
                switch (lookahead()) {
                    case TokenType::l_paren:
                        _context->syntaxTree->add(parseFunction());
                        break;
                    case TokenType::comma:
                    case TokenType::semi:
                    case TokenType::op_eq:
                    case TokenType::l_square:
                        _context->syntaxTree->add(parseVariableDeclaration());
                        break;
                    default:
                        break;
                }
            } else {
                // let's push since we don't know what's happening here
                _diagnosticStream <<
                                  diagnostic::Diagnostic(diagnostic::Error, _context, "unexpected token(s)")
                                          .at(_current);
                bool searching = true;
                while (searching && _current < _context->tokens.size()) {
                    step();
                    switch (current()) {
                        // stop parsing at eof
                        // continue parsing at kw_const
                        case ast::TokenType::eof:
                        case ast::TokenType::kw_const:
                            searching = false;
                            break;
                        case ast::TokenType::identifier: {
                            prepareLookahead();
                            if (lookahead() == ast::TokenType::identifier) {
                                move(_lookahead);
                                switch (lookahead()) {
                                    case ast::TokenType::op_eq:
                                    case ast::TokenType::semi:
                                    case ast::TokenType::l_paren:
                                    case ast::TokenType::l_square:
                                    case ast::TokenType::comma:
                                        searching = false;
                                        break;
                                    default:
                                        break;
                                }
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }
                //
                prepareLookahead();
            }
        }
    }

    std::shared_ptr<ast::syntax::Identifier> Parser::parseIdentifier() {
        assert(current() == ast::TokenType::identifier);
        auto id = std::make_shared<Identifier>();
        id->identifier = (std::string) _context->tokens[_current]->raw_code;
        id->mangledId = id->identifier;
        id->tokenIndex = _current;
        step();
        return id;
    }

    std::shared_ptr<ast::syntax::Function> Parser::parseFunction() {
        recover();
        auto function = std::make_shared<Function>();
        function->returnType = parseType();
        function->name = parseIdentifier();
        // we assume that all subroutines that calls this should
        assert(current() == ast::TokenType::l_paren);
        function->lParenIndex = _current;
        step();
        while (current() == ast::TokenType::identifier) {
            auto type = parseType();
            auto para = parseVariableDefinition();
            para->type = type;
            function->params.push_back(para);
            if (current() == ast::TokenType::comma)
                step();
        }
        if (current() != ast::TokenType::r_paren) {
            _diagnosticStream << diagnostic::Diagnostic(diagnostic::Error, _context, "expected $ before token $")
                    .at(_current)
                    .emphasize(")")
                    .emphasize((std::string) _context->tokens[_current]->raw_code)
                              << diagnostic::Diagnostic(diagnostic::Note, _context, "to match the paren here")
                                      .at(function->lParenIndex);
        } else {
            function->rParenIndex = _current;
            step();
        }
        if (current() == ast::TokenType::semi)
            return function;

        if (current() != ast::TokenType::l_brace) {
            if (current() == ast::TokenType::eof){
                _diagnosticStream << diagnostic::Diagnostic(diagnostic::Error, _context, "$ expected after token $")
                        .at(_current - 1)
                        .emphasize(";")
                        .emphasize((std::string) _context->tokens[_current - 1]->raw_code);
            } else{
                _diagnosticStream << diagnostic::Diagnostic(diagnostic::Error, _context, "$ expected before token $")
                        .at(_current)
                        .emphasize(";")
                        .emphasize((std::string) _context->tokens[_current]->raw_code);
            }
            function->body = nullptr;
        } else {
            function->body = parseBlockStatement();
        }
        return function;
    }

    std::shared_ptr<ast::syntax::Expression>
    Parser::parseExpression(const std::set<ast::TokenType> &endTokens, OperatorPrecedence parentPrecedence) {
        prepareLookahead();
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
                case ast::TokenType::string_literal: {
                    auto number = std::make_shared<StringLiteralExpression>();
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
                    auto indexed = std::make_shared<IndexExpression>();
                    if (left->getType() == ast::syntax::SyntaxType::IndexExpression){
                        auto indexExpr = std::dynamic_pointer_cast<ast::syntax::IndexExpression>(left);
                        indexed->arrayName = indexExpr->arrayName;
                        indexed->layer = indexExpr->layer + 1;
                    } else {
                        indexed->arrayName = std::dynamic_pointer_cast<ast::syntax::IdentifierExpression>(left)->name;
                    }
                    if (current() == TokenType::r_square) {
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
        return move(_current) && prepareLookahead();
    }

    bool Parser::prepareLookahead() {
        _lookahead = _current;
        return move(_lookahead);
    }

    std::shared_ptr<ast::syntax::VarDeclaration> Parser::parseVariableDeclaration() {
        recover();
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
                auto def = parseVariableDefinition();
                def->type = decl->type;
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


    std::shared_ptr<ast::syntax::VarDefinition> Parser::parseVariableDefinition() {
        auto def = std::make_shared<VarDefinition>();
        def->varName = parseIdentifier();
        bool varEnd = false;
        do {
            switch (current()) {
                case ast::TokenType::l_square: {
                    step();
                    if (current() == ast::TokenType::r_square) {
                        step();
                        def->array.push_back(nullptr);
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
                case ast::TokenType::r_paren:
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
        return def;
    }

    std::shared_ptr<ast::syntax::Type> Parser::parseType() {
        auto  type = std::make_shared<ast::syntax::Type>();
        type->typeName = parseIdentifier();
        return type;
    }

    void Parser::pushRecover() {
        _recover.push(_current);
    }

    bool Parser::pushRecoverAndStep() {
        _recover.push(_current);
        return step();
    }

    void Parser::recover() {
        if (!_recover.empty()) {
            size_t pos = _recover.top();
            _recover.pop();
            _current = pos;
        }
        prepareLookahead();
    }
}
