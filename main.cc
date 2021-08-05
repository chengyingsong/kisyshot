#include <iostream>
#include <memory>
#include <filesystem>
#include <set>

#include "ast/syntax/syntax_ostream_writer.h"
#include "context_manager.h"
#include "ast/cfg.h"
#include "ast/arms.h"
#include "compiler/armcode.h"

using namespace kisyshot::ast;

int main() {
    auto sm = std::make_shared<kisyshot::ContextManager>();
    std::set<std::string> paths;
    for (const auto& entry: std::filesystem::directory_iterator("cases/function_test2020")) {
        if(entry.path().extension() == ".sy") {
            paths.insert(entry.path().string());
        }
    }
    for(const auto& entry: paths){
        std::cout << entry << std::endl;
        auto ctx = sm->load(entry);
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
