#include <algorithm>
#include <context.h>

namespace kisyshot {
    Context::Context(const std::string_view &code,
                     std::size_t contextID) {
        this->code = code;
        this->contextID = contextID;
        // initialize lines:
        std::size_t lastLine = 0;
        for (std::size_t i = 0; i < code.size(); ++i) {
            if (code[i] == '\n') {
                lineStartPos.push_back(lastLine);
                lines.push_back(code.substr(lastLine, i - lastLine));
                lastLine = i + 1;
            }
        }
        // handle with
        // "....str"
        //         ^  not end with \n!
        if (lastLine != code.size()) {
            lineStartPos.push_back(lastLine);
            lines.push_back(code.substr(lastLine));
        }
    }

    CodePosition Context::locate(const ast::Token &token) const {
        auto startLineIndex = std::upper_bound(lineStartPos.begin(), lineStartPos.end(), token.offset);
        size_t id = startLineIndex - lineStartPos.begin();
        return CodePosition{id, token.offset - lineStartPos[id - 1]};
    }

    CodePosition Context::locate(std::size_t index) const {
        return locate(*tokens[index]);
    }

    std::size_t Context::firstOfLine(std::size_t line) const {
        auto cmp = std::make_unique<kisyshot::ast::Token>();
        cmp->offset = lineStartPos[line];
        size_t idx =
                std::lower_bound(tokens.begin(), tokens.end(), cmp,
                                 [](const std::unique_ptr<kisyshot::ast::Token> &t1,
                                    const std::unique_ptr<kisyshot::ast::Token> &t2) -> bool {
                                     return t1->offset < t2->offset;
                                 }) - tokens.begin();
        if (locate(idx).line - 1 != line)
            return npos;
        return idx;
    }

    std::size_t Context::lastOfLine(std::size_t line) const {
        while (line < lines.size() - 1) {
            size_t nextStart = firstOfLine(line + 1);
            if (nextStart != npos) {
                if (locate(nextStart - 1).line - 1== line)
                    return nextStart - 1;
                else
                    return npos;
            }
            line++;
        }
        return tokens.size() - 1;
    }
}