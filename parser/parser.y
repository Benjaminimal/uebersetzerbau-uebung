%token ID NUM END RETURN IF THEN ELSE LOOP BREAK CONT VAR NOT AND

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
      Pars_r
    | ID ',' Pars
    ;
Pars_r:
      /* empty */
    | ID
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
      Expr_list_r
    | Expr ',' Expr_list
    ;
Expr_list_r:
      /* empty */
    | Expr
    ;
