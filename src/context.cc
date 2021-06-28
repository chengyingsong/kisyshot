#include <algorithm>
#include <context.h>

namespace kisyshot {
    Context::Context(const std::string_view &code,
                     std::size_t syntaxID) {
        this->code = code;
        this->syntaxID = syntaxID;
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

    CodePosition Context::locate(const ast::Token &token) {
        auto startLineIndex = std::upper_bound(lineStartPos.begin(), lineStartPos.end(), token.offset);
        size_t id = startLineIndex - lineStartPos.begin();
        return CodePosition{.line = id, .offset = token.offset - lineStartPos[id - 1]};
    }

    CodePosition Context::locate(std::size_t index) {
        return locate(*tokens[index]);
    }
}