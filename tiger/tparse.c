#include <tiger/tparse.h>
#include <stdio.h>
#include <malloc.h>
#include <gmp.h>

tgToken* look = NULL;
int error = 0;

static void tgMove(tgEnv* env, tgLexer* lex, FILE* infile) {
  look = tgLexer_scan(env, lex, infile);
}

static void tgError(char const* err) {
  printf("%s\n", err);
  error = 1;
}

static void tgMatch(tgEnv* env, tgLexer* lex, FILE* infile, int t) {
  if(look->tag == t) tgMove(env, lex, infile);
  else tgError("Lexical error!");
}

typedef struct tgStmt_ {
  struct tgStmt_* next;
  void (*exec)(void*, void*);
  void *lhs, *rhs;
} tgStmt;

mpf_t eax;

void tgConst_print(void* a, void* b) {
  gmp_printf("%.Ff\n", *(mpf_t*)a);
}

void tgExpr_add(void*a, void*b) {
  mpf_t* lhs = (mpf_t*)a;
  mpf_t* rhs = (mpf_t*)b;
  mpf_add(eax, *lhs, *rhs);
}

void tgExpr_sub(void*a, void*b) {
  mpf_t* lhs = (mpf_t*)a;
  mpf_t* rhs = (mpf_t*)b;
  mpf_sub(eax, *lhs, *rhs);
}

void tgExpr_mul(void*a, void*b) {
  mpf_t* lhs = (mpf_t*)a;
  mpf_t* rhs = (mpf_t*)b;
  mpf_mul(eax, *lhs, *rhs);
}

void tgExpr_div(void*a, void*b) {
  mpf_t* lhs = (mpf_t*)a;
  mpf_t* rhs = (mpf_t*)b;
  mpf_div(eax, *lhs, *rhs);
}

static tgStmt* tgConst_match(tgEnv* env, tgLexer* lex, FILE* in, tgToken* tok) {
  tgStmt* stmt = malloc(sizeof(tgStmt));
  stmt->lhs = tgEnv_getSym(env, ((tgTId*)tok)->name)->data;
  stmt->exec = &tgConst_print;
  return stmt;
}

static tgStmt* tgPrint_create() {
  tgStmt* stmt = malloc(sizeof(tgStmt));
  stmt->lhs = eax;
  stmt->exec = &tgConst_print;
}

static tgStmt* tgExpr_match(tgEnv* env, tgLexer* lex, FILE* in, tgToken* tok) {
  tgStmt* stmt = malloc(sizeof(tgStmt));
  tgToken* op = look; tgMove(env, lex, in);
  stmt->lhs = tgEnv_getSym(env, ((tgTId*)tok)->name)->data;
  stmt->rhs = tgEnv_getSym(env, ((tgTId*)look)->name)->data;
  switch (*((tgTId*)op)->name) {
    case '+':
      stmt->exec = &tgExpr_add;
      break;
    case '-':
      stmt->exec = &tgExpr_sub;
      break;
    case '/':
      stmt->exec = &tgExpr_div;
      break;
    case '*':
      stmt->exec = &tgExpr_mul;
      break;
  }
  stmt->next = tgPrint_create();
  return stmt;
}

static tgStmt* tgId_match(tgEnv* env, tgLexer* lex, FILE* in) {
  tgStmt* stmt = NULL;
  tgToken* prev = look;
  tgMove(env, lex, in);
  switch (look->tag) {
    case TG_OPERATOR:
      switch (*((tgTId*)look)->name) {
        case '=':
          //return tgAssign_match(env, lex, in, prev);
        case '+':
        case '-':
        case '/':
        case '*':
          stmt = tgExpr_match(env, lex, in, prev);
          return stmt;
        default:
          //error(1, 3, "Invalid Format!");
          return NULL;
      }
    case TG_TERMINAL:
      if(((tgTTerm*)look)->nonterm == ';') {
        return tgConst_match(env, lex, in, prev);
      }
  }
  return NULL;
}

static tgStmt* tgStmt_match(tgEnv* env, tgLexer* lex, FILE* in) {
  for(;;) {
    switch (look->tag) {
      case TG_ID:
        //return tgSet_match(env, lex, in);
      case TG_REAL:
        //return tgConst_match(env, lex, in);
      default:
        return NULL;
    }
  }
  return NULL;
}

void tgStmt_exec(tgStmt* stmt) {
  while (stmt) {
    stmt->exec(stmt->lhs, stmt->rhs);
    stmt = stmt->next;
  }
}

void tgParser_parse(tgEnv* env, tgParser* parser, tgLexer* lexer, FILE* infile) {
  tgStmt* stmt;
  mpf_init(eax);
  for(;;) {
  tgMove(env, lexer, infile);
    switch(((tgTTerm*)look)->nonterm) {
      case EOF:
        return;
      case ';':
        continue;
      default:
        stmt = tgId_match(env, lexer, infile);
        if (stmt) tgStmt_exec(stmt);
        break;
    }
  }
}

tgClass tgParser_class = {
  sizeof(tgParser), &tgNull_ctor, &tgNull_dtor
};
