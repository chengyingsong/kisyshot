#include "ast/syntax/syntax_ostream_writer.h"
#include "context_manager.h"
#include "doctest.h"

using namespace kisyshot;
using namespace kisyshot::ast;
using namespace kisyshot::diagnostic;

TEST_CASE("sema_symbols"){
    auto sm = std::make_shared<kisyshot::ContextManager>();
    sm->load("cases/sema/symbol_test.sy");
    sm->lex(0);
    sm->parse(0);
    sm->check(0);
    
    std::cout << *(sm->operator[](0)->syntaxTree);
    REQUIRE(1 == 1);
}