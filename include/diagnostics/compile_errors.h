#pragma once

namespace kisyshot::diagnostics{
    // Pre-defined compile errors for SysY compiler.
    enum class CompileError{
        // KS1000-1999: lexical errors
        UnexpectedChars = 1000,
        StringNotClosed,
        InvalidNumericConst,
        InterlineCommentNotClosed,
        // KS2000-2999: parse errors
        UnexpectedToken = 2000,
        SemiExpected,
        CommaExpected,
        LBraceExpected,
        RBraceExpected,
        LParenExpected,
        RParenExpected,
        RSquareExpected,
        GreaterExpected,
        IdentifierExpected,
        ImportExpected,
        ColonExpected,

    };
}