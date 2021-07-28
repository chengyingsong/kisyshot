#include <compiler/parser.h>
#include <compiler/lexer.h>
#include <filesystem>
#include "ast/syntax/syntax_ostream_writer.h"
#include "context_manager.h"
#include "doctest.h"

using namespace kisyshot;
using namespace kisyshot::ast;
using namespace kisyshot::compiler;
using namespace kisyshot::diagnostic;

TEST_CASE("parse_function_fails") {
    auto sm = std::make_shared<kisyshot::ContextManager>();
    for (const auto& entry: std::filesystem::directory_iterator("cases/parse/function")) {
        auto ctx = sm->load(entry.path());
        Lexer(ctx, sm->diagnosticStream).lex();
        std::cout << *Parser(ctx, sm->diagnosticStream).parseFunction();
        std::cout << *(sm->diagnosticStream);
    }
}