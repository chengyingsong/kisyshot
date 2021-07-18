#include <rang/rang.h>
#include <iostream>
#include <diagnostics/diagnostic.h>

namespace kisyshot::diagnostics {
    ErrorAtDiagnostic::ErrorAtDiagnostic(CompileError error, const std::shared_ptr<Context> &ctx,
                                         std::size_t tokenID,
                                         const std::string &message, const std::string &suggestion) {
        errorCode = error;
        context = ctx;
        _tokenID = tokenID;
        _message = message;
        _suggestion = suggestion;
    }

    void
    ErrorAtDiagnostic::writeTo(std::ostream &stream) {
        size_t tokenLength = context->tokens[_tokenID]->raw_code.length();

        auto pos = context->locate(_tokenID);
        std::string path = context->path;

        auto isCout = stream.rdbuf() == std::cout.rdbuf();
        if (isCout) {
            // write to std::cout, we should make it colorful
            stream << path << ":" << pos.line << ":" << pos.offset << ": "
                   << rang::fg::red << "error: " << rang::fg::reset << _message << std::endl
                   << context->lines[pos.line - 1] << std::endl
                   // prepare for tip mark color
                   << rang::fg::green;
        } else {
            stream << path << ":" << pos.line << ":" << pos.offset << ": "
                   << "error: " << _message << std::endl
                   << context->lines[pos.line - 1] << std::endl;
        }

        // get the position of the first char in the tokens
        for (size_t sIdx = 0; sIdx < pos.offset; sIdx++) {
            stream << " ";
        }
        stream << "^";
        // since tokens like interline-comments and string literals
        // can take multi lines of code, to make sure that tildes have
        // the right end position
        size_t tildeNums = std::min(context->lines[pos.line - 1].length() - pos.offset,
                                    tokenLength);
        for (std::size_t i = 1; i < tildeNums; ++i) {
            stream << "~";
        }
        // reset color if it's write to std::cout
        if (isCout)
            stream << rang::fg::reset;
        stream << std::endl;
        if (!_suggestion.empty()) {
            stream << _suggestion << std::endl;
        }
    }

    ErrorAfterDiagnostic::ErrorAfterDiagnostic(CompileError error, const std::shared_ptr<Context> &ctx,
                                               std::size_t tokenID,
                                               const std::string &message, const std::string &suggestion) {
        errorCode = error;
        context = ctx;
        _tokenID = tokenID;
        _message = message;
        _suggestion = suggestion;
    }

    void ErrorAfterDiagnostic::writeTo(std::ostream &stream) {
        size_t tokenLength = context->tokens[_tokenID]->raw_code.length();

        auto pos = context->locate(_tokenID);
        std::string path = context->path;

        auto isCout = stream.rdbuf() == std::cout.rdbuf();

        if (isCout) {
            // write to std::cout, we should make it colorful
            stream << path << ":" << pos.line << ":" << pos.offset << ": "
                   << rang::fg::red << "error: " << rang::fg::reset << _message << std::endl
                   << context->lines[pos.line - 1] << std::endl
                   // prepare for tip mark color
                   << rang::fg::green;
        } else {
            stream << path << ":" << pos.line << ":" << pos.offset << ": "
                   << "error: " << _message << std::endl
                   << context->lines[pos.line - 1] << std::endl;
        }

        for (size_t sIdx = 0; sIdx < pos.offset + tokenLength; sIdx++) {
            stream << " ";
        }
        stream << "^";
        // reset color if it's write to std::cout
        if (isCout)
            stream << rang::fg::reset;
        stream << std::endl;
        if (!_suggestion.empty()) {
            stream << _suggestion << std::endl;
        }
    }

    ErrorBeforeDiagnostic::ErrorBeforeDiagnostic(CompileError error, const std::shared_ptr<Context> &ctx,
                                                 std::size_t tokenID,
                                                 const std::string &message, const std::string &suggestion) {
        errorCode = error;
        context = ctx;
        _tokenID = tokenID;
        _message = message;
        _suggestion = suggestion;
    }

    void ErrorBeforeDiagnostic::writeTo(std::ostream &stream) {
        auto pos = context->locate(_tokenID);
        std::string path = context->path;

        auto isCout = stream.rdbuf() == std::cout.rdbuf();

        if (isCout) {
            // write to std::cout, we should make it colorful
            stream << path << ":" << pos.line << ":" << pos.offset << ": "
                   << rang::fg::red << "error: " << rang::fg::reset << _message << std::endl
                   << context->lines[pos.line - 1] << std::endl
                   // prepare for tip mark color
                   << rang::fg::green;
        } else {
            stream << path << ":" << pos.line << ":" << pos.offset << ": "
                   << "error: " << _message << std::endl
                   << context->lines[pos.line - 1] << std::endl;
        }

        for (size_t sIdx = 0; sIdx + 1 < pos.offset; sIdx++) {
            stream << " ";
        }
        stream << "^";
        // reset color if it's write to std::cout
        if (isCout)
            stream << rang::fg::reset;
        stream << std::endl;
        if (!_suggestion.empty()) {
            stream << _suggestion << std::endl;
        }
    }
}
