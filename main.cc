#include <memory>
#include <filesystem>

#include "ast/syntax/syntax_ostream_writer.h"
#include "context_manager.h"
#include "ast/cfg.h"
#include "ast/arms.h"
#include "compiler/armcode.h"

using namespace kisyshot::ast;

int main(int argc, char* argv[]) {
    if (argc == 5) {
        auto sm = std::make_shared<kisyshot::ContextManager>();
        auto ctx = sm->load(std::string(argv[4]));
        ctx->target = argv[3];
        sm->lex(ctx->contextID);
        sm->parse(ctx->contextID);
        sm->check(ctx->contextID);
        kisyshot::compiler::CodeGenerator gen;
        ctx->syntaxTree->genCode(gen, nullptr);
        kisyshot::compiler::ArmCodeGenerator armgen(gen.code, ctx);
        armgen.generateArmCode();
    }
    return 0;
}
