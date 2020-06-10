#ifndef _EXIT_H_
#define _EXIT_H_

#define ERR_LEX 1
#define ERR_PARSE 2
#define ERR_SCOPE 3
#define ERR_OOM 4

#define EXIT_ERR_LEX() \
    fprintf( \
        stderr, \
        "%d: lexical error: unexpected character '%s'\n", \
        yylineno, \
        yytext \
    ); \
    exit(ERR_LEX);

#define EXIT_ERR_PARSE(msg) \
    fprintf( \
        stderr, \
        "%d: %s\n", \
        yylineno, \
        msg \
    ); \
    exit(ERR_PARSE);

#define EXIT_ERR_DUPLICATE_ID(lexeme) \
    fprintf( \
        stderr, \
        "%d: scope error, %s already defined\n", \
        yylineno, \
        lexeme \
    ); \
    exit(ERR_SCOPE);

#define EXIT_ERR_UNDEFINED_ID(lexeme) \
    fprintf( \
        stderr, \
        "%d: scope error, %s undefined\n", \
        yylineno, \
        lexeme \
    ); \
    exit(ERR_SCOPE);

#define EXIT_ERR_OOM() \
    fprintf( \
        stderr, \
        "out of memory\n" \
    ); \
    exit(ERR_OOM);

#endif /* _EXIT_H_ */
