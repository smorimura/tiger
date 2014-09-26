#include <tiger/tparse.h>
#include <stdio.h>

tgToken* look = NULL;
int error = 0;

static void tgMove(tgLexer* lex, FILE* infile) {
  look = tgLexer_scan(lex, infile);
}

static void tgError(char const* err) {
  printf("%s\n", err);
  error = 1;
}

static void tgMatch(tgLexer* lex, FILE* infile, int t) {
  if(look->tag == t) tgMove(lex, infile);
  else tgError("Lexical error!");
}

void tgParser_parse(tgParser* parser, tgLexer* lexer, FILE* infile) {
  tgMove(lexer, infile);
  // tgStatement stmt;
  while(look) {
    switch (look->tag) {
      case TG_ID:
        printf("Analyzed ID:   %s\n", ((tgTId*)look)->name);
        break;
      case TG_REAL:
        gmp_printf("Analyzed Real: %.Ff\n", ((tgTReal*)look)->value);
        break;
      case TG_OPERATOR:
        printf("Analyzed Oper: %s\n", ((tgTOperator*)look)->string);
        break;
      case TG_TERMINAL:
        printf("Analyzed Term: %c\n", ((tgTTerm*)look)->nonterm);
        switch(((tgTTerm*)look)->nonterm) {
          case EOF:
            return;
          case ';':
            // tgStatement_exec(stmt);
            break;
        }
        break;
    }
    tgMove(lexer, infile);
  }
}

tgClass tgParser_class = {
  sizeof(tgParser), &tgNull_ctor, &tgNull_dtor
};
