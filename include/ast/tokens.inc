// Hack codes by using marcos to generate some repeated codes and prevent typos
// This file mostly defines the special tokens in the SysY language.
// We can add SPECIAL_TOK_CLASSIFY(params) to add new special Token types.
#ifndef TOKEN
#define TOKEN(X)
#endif

/*
 * PUNCTUATOR:
 *   punctuator is a special of Token, mostly used for split purposes
 */
#ifndef PUNCTUATOR
#define PUNCTUATOR(X,Y) TOKEN(X)
#endif

/*
 * OPERATOR:
 *   operator is a special Token, mostly used to indicate the operation
 *   of identifier elements
 */
#ifndef OPERATOR
#define OPERATOR(X,Y) TOKEN(op_ ## X)
#endif

/*
 * KEYWORD:
 *   keyword is a special Token, mostly reserved to represent syntax
 */
#ifndef KEYWORD
#define KEYWORD(X) TOKEN(kw_ ## X)
#endif


// compiler-internal tokens
TOKEN(unknown)              // unknown Token for yuuki
TOKEN(eof)                  // end of file
TOKEN(inline_comment)       // in-line comment
TOKEN(interline_comment)    // inter-line comment


// yuuki language user input tokens
TOKEN(identifier)           // identifier like: _some_variable
TOKEN(numeric_literal)      // numeric const like: 01237
TOKEN(string_literal)       // string const like: "abc"

// operator definitions
PUNCTUATOR(l_paren,     "(")
PUNCTUATOR(r_paren,     ")")
PUNCTUATOR(l_square,    "[")
PUNCTUATOR(r_square,    "]")
PUNCTUATOR(l_brace,     "{")
PUNCTUATOR(r_brace,     "}")
PUNCTUATOR(comma,       ",")
PUNCTUATOR(semi,        ";")

// operator definitions
// assignment
OPERATOR(eq,            "=")
// mathematical ops
OPERATOR(plus,          "+")
OPERATOR(minus,         "-")
OPERATOR(multi,         "*")
OPERATOR(slash,         "/")
OPERATOR(modulus,       "%")
// relationship ops
OPERATOR(less,          "<")
OPERATOR(greater,       ">")
OPERATOR(equaleq,       "==")
OPERATOR(exclaimeq,     "!=")
OPERATOR(greatereq,     ">=")
OPERATOR(lesseq,        "<=")
// logical ops
OPERATOR(exclaim,       "!")
OPERATOR(ampamp,        "&&")
OPERATOR(pipepipe,      "||")



// keywords
KEYWORD(if)
KEYWORD(else)
KEYWORD(while)
KEYWORD(continue)
KEYWORD(break)
KEYWORD(return)
KEYWORD(const)


#undef TOKEN
#undef PUNCTUATOR
#undef KEYWORD
#undef OPERATOR