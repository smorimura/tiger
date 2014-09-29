#include <tiger/tparse.h>
#include <stdio.h>
#include <malloc.h>
#include <gmp.h>

tgToken* look = NULL;
int error = 0;
int didset = 0;

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

static void tgFunc_call(void* a, void* b) {
  didset = 1;
  void (*func)() = a;
  func();
}

static tgStmt* tgFunc_match(tgEnv* env, tgLexer* lex, FILE* in, tgToken* tok) {
  tgMove(env, lex, in);
  tgMatch(env, lex, in, TG_TERMINAL);
  tgStmt* stmt = malloc(sizeof(tgStmt));
  stmt->lhs = tgEnv_getSym(env, ((tgTId*)tok)->name)->data;
  stmt->exec = &tgFunc_call;
  return stmt;
}

static void tgEnv_set(void*a, void*b) {
  mpf_set(eax, *(mpf_t*)a);
}

static tgStmt* tgConst_match(tgEnv* env, tgLexer* lex, FILE* in, tgToken* tok) {
  tgStmt* stmt = malloc(sizeof(tgStmt));
  stmt->lhs = tgEnv_getSym(env, ((tgTId*)tok)->name)->data;
  stmt->exec = &tgEnv_set;
  return stmt;
}

static tgStmt* tgPrint_create() {
  tgStmt* stmt = malloc(sizeof(tgStmt));
  stmt->lhs = eax;
  stmt->exec = &tgConst_print;
}

static void tgExpr_set(void* a, void* b) {
  didset = 1;
  mpf_set(*(mpf_t*)a, *(mpf_t*)b);
}

typedef struct {
  tgStmt stmt;
  int pres;
} tgExpr;

static tgExpr* tgExpr_match(tgEnv* env, tgLexer* lex, FILE* in, tgToken* tok, tgExpr** last) {
  tgExpr* stmt = malloc(sizeof(tgExpr));
  tgExpr* stmt2 = NULL;
  tgExpr* final = stmt;
  tgToken* op = look; tgMove(env, lex, in);
  stmt->stmt.lhs = tgEnv_getSym(env, ((tgTId*)tok)->name)->data;
  stmt->stmt.rhs = tgEnv_getSym(env, ((tgTId*)look)->name)->data;
  switch (*((tgTId*)op)->name) {
    case '+':
      stmt->pres = 0;
      stmt->stmt.exec = &tgExpr_add;
      break;
    case '-':
      stmt->pres = 0;
      stmt->stmt.exec = &tgExpr_sub;
      break;
    case '/':
      stmt->pres = 1;
      stmt->stmt.exec = &tgExpr_div;
      break;
    case '*':
      stmt->pres = 1;
      stmt->stmt.exec = &tgExpr_mul;
      break;
    case '=':
      stmt->pres = 2;
      stmt->stmt.exec = &tgExpr_set;
      break;
  }
  if(last) *last = NULL;

  // See if this is a more complex expression.
  tgToken* stmtr = look;
  tgMove(env, lex, in);
  if (look->tag == TG_OPERATOR) {
    switch(*((tgTId*)look)->name) {
      case '+':
      case '=':
      case '-':
      case '/':
      case '*': {
        tgExpr* parent = NULL;
        tgExpr* leftof = NULL;
        stmt2 = tgExpr_match(env, lex, in, stmtr, &parent);
        if(parent) leftof = parent->stmt.next; else leftof = stmt2;
        if (stmt2) {
          if (stmt->pres >= leftof->pres) {
            if(!parent) {
              final = stmt;
              final->stmt.next = stmt2;
              stmt2->stmt.lhs = eax;
            } else {
              stmt->stmt.next = leftof;
              parent->stmt.next = stmt;
              if(last) *last = parent;
              stmt->stmt.rhs = eax;
            }
          } else {
            final = stmt2;
            while(leftof->stmt.next) leftof = leftof->stmt.next;
            leftof->stmt.next = stmt;
            stmt->stmt.rhs = eax;
          }
        }
      }
        break;
      default:
        break;
    }
  }
  return final;
}

static tgStmt* tgStmt_match(tgEnv* env, tgLexer* lex, FILE* in) {
  tgStmt* stmt = NULL;
  tgToken* prev = look;

  // Peek at the next token to see stmt/expr
  tgMove(env, lex, in);
  switch (look->tag) {
    case TG_OPERATOR:
      switch (*((tgTId*)look)->name) {
        case '=':
        case '+':
        case '-':
        case '/':
        case '*':
          stmt = (tgStmt*)tgExpr_match(env, lex, in, prev, NULL);
          return stmt;
        default:
          //error(1, 3, "Invalid Format!");
          return NULL;
      }
    case TG_TERMINAL:
      if(((tgTTerm*)look)->nonterm == ';') {
        return tgConst_match(env, lex, in, prev);
      } else if(((tgTTerm*)look)->nonterm == '(') {
        return tgFunc_match(env, lex, in, prev);
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
    if(infile == stdin) printf("> ");
    tgMove(env, lexer, infile);
    switch(((tgTTerm*)look)->nonterm) {
      case EOF:
        return;
      case ';':
      case '\n':
        continue;
      default:
        didset = 0;
        stmt = tgStmt_match(env, lexer, infile);
        if (stmt) tgStmt_exec(stmt);
        if (!didset) {
          gmp_printf("%.Ff\n", eax);
        }
        break;
    }
  }
}

tgClass tgParser_class = {
  sizeof(tgParser), &tgNull_ctor, &tgNull_dtor
};
