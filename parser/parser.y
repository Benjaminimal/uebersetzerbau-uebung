//
%locations
%define api.pure full

%code top{
#include <stdlib.h>
#include <stdio.h>

#define YYERROR_VERBOSE 1
}

%code {
void yyerror(YYLTYPE *locp, char const *msg);
extern int yylex(YYSTYPE *lvalp, YYLTYPE *llocp);
}

%token ID
%token NUM
%token END
%token RETURN
%token IF
%token THEN
%token ELSE
%token LOOP
%token BREAK
%token CONT
%token VAR
%token NOT
%token AND
%token ASSIGN ":="
%token LEQ "<="

%start Program

%%

Program:
      /* empty */
    | Program Funcdef ';'
    ;

Funcdef:
      ID '(' Pars ')' Stats END
    ;

Pars:
      /* empty */
    | ID
    | ID ',' Pars
    ;

Stats:
      /* empty */
    | Stat ';' Stats
    ;

Stat:
      RETURN Expr
    | IF Expr THEN Stats Else END
    | ID ':' LOOP Stats END
    | BREAK ID
    | CONT ID
    | VAR ID ":=" Expr
    | Lexpr ":=" Expr
    | Expr
    ;

Else:
      /* empty */
    | ELSE Stats
    ;

Lexpr:
       ID
     | '*' Term
     ;

Expr:
    | Unary Term
    | Term Expr_add
    | Term Expr_mul
    | Term Expr_and
    | Term "<=" Term
    | Term '#' Term
    ;
Expr_add:
      /* empty */
    | '+' Term Expr_add
    ;
Expr_mul:
      /* empty */
    | '*' Term Expr_mul
    ;
Expr_and:
      /* empty */
    | AND Term Expr_and
    ;

Unary:
      /* empty */
    | NOT Unary
    | '-' Unary
    | '*' Unary
    ;

Term:
      '(' Expr ')'
    | NUM
    | ID
    | ID '(' Expr_list ')'
    ;

Expr_list:
      /* empty */
    | Expr
    | Expr ',' Expr_list
    ;

%%

void yyerror(YYLTYPE *locp, char const *msg) {
    (void) fprintf(
        stderr,
        "%d:%d: %s\n",
        locp->first_line,
        locp->first_column,
        msg
    );
    exit(2);
}

int main(int argc, char *argv[]) {
    (void) yyparse();
    return 0;
}
