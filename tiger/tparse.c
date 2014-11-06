/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tparse.h>
#include <tiger/talloc.h>
#include <tiger/tlex.h>
#include <tiger/tcode.h>
#include <tiger/tenv.h>
#include <tiger/tstate.h>
#include <malloc.h>
#include <assert.h>

#define tgTokenN(p,n) p->tokens[n]
#define tgToken(p) tgTokenN(p,0)
#define tgAttribN(p,n) tgTokenN(p,n).attrib
#define tgAttrib(p) tgAttribN(p,0)
#define tgTagN(p,n) tgTokenN(p,n).tag
#define tgTag(p) tgTagN(p,0)

static void tgParser_load(tgParser* p) {
  size_t off;
  switch(tgTag(p)) {
    case TG_INTEGER:
      off = tgCode_defInt(p->code, tgAttrib(p).integer);
      break;
    case TG_REAL:
      off = tgCode_defReal(p->code, tgAttrib(p).real);
      break;
    case TG_IDENTIFIER:
      off = tgCode_defSym(p->code, tgAttrib(p).string);
      tgCode_writeA(p->code, TB_PUSH, off);
      tgLexer_parse(p);
      return;
      break;
    case TG_STRING:
      off = tgCode_defStr(p->code, tgAttrib(p).string);
    default:
      break;
  }
  tgCode_loadk(p->code, *ridx, off);
  tgLexer_parse(p);
}

static size_t tgParser_identPrec(tgTag t) {
  switch (t) {
    case TG_DIV:
    case TG_MUL:
      return 50;
    case TG_ADD:
    case TG_SUB:
      return 20;
    case TG_EQ:
    case TG_LT:
    case TG_LE:
    case TG_GT:
    case TG_GE:
      return 10;
    default:
      return 0;
  }
}

static void tgParser_writeOp(tgParser* p, tgTag op) {
  tgByteCode c;
  switch (op) {
    case TG_MUL:
      c = TB_MUL;
      break;
    case TG_DIV:
      c = TB_DIV;
      break;
    case TG_ADD:
      c = TB_ADD;
      break;
    case  TG_SUB:
      c = TB_SUB;
      break;
    case TG_EQ:
      c = TB_EQ;
      break;
    case TG_LE:
      c = TB_LE;
      break;
    case TG_LT:
      c = TB_LT;
      break;
    case TG_GT:
      c = TB_GT;
      break;
    case TG_GE:
      c = TB_GE;
      break;
    default:
      break;
  }
  tgCode_write(p->code, c);
}

static size_t tgParser_expr(tgParser* p, size_t lhp) {
  tgParser_load(p);
  {
    tgTag  nx, op = tgTag(p);
    size_t pn, pr = tgParser_identPrec(op);
    while (op != TG_ENDSTMT && pr > lhp) {
      tgLexer_parse(p);
      nx = tgTagN(p,1);
      pn = tgParser_identPrec(nx);
      if (pn > pr) {
        nx = tgParser_expr(p, pr);
        tgParser_writeOp(p, op);
        if (nx == TG_ENDSTMT) return nx;
        tgLexer_parse(p);
        op = nx;
      }
      tgParser_load(p);
      tgParser_writeOp(p, op);
      op = tgTag(p);
      pr = tgParser_identPrec(op);
    }
    return op;
  }
}

static void tgParser_exprAssign(tgParser* p, size_t symoff) {
  tgCode_writeA(p->code, TB_PUSHR, symoff);
  tgParser_expr(p, 0);
  tgCode_write(p->code, TB_ASSIGN);
}

static void tgParser_assign(tgParser* p, size_t symoff) {
  tgLexer_parse(p); /* {:=} {K,S} ?? -> {K,S} {;,OP} ?? */

  /* Check for trivial assignment */
  switch (tgTagN(p,1)) {
    default:
      tgParser_exprAssign(p, symoff);
      break;
  }
}

static void tgParser_call(tgParser* p, size_t off) {
  tgLexer_parse(p);
  while (tgTag(p) != TG_RPARENS) {
    tgParser_expr(p, 0);
  }
  tgCode_writeA(p->code, TB_CALL, off);
  tgLexer_parse(p);
}

static void tgParser_stmtBody(tgParser* p) {
  /* Must peek to see if we're defining a new local variable */
  size_t off;
  switch (tgTagN(p,1)) {
    /* Note: This is where we would add scoping if we choose to. {} */
    case TG_DEFINE:
    case TG_ASSIGN:
      off = tgCode_defSym(p->code, tgToken(p).attrib.string);
      tgLexer_parse(p); /* AA := ?? -> := {K,S} ?? */
      tgParser_assign(p, off);
      break;
    case TG_LPARENS:
      off = tgCode_defSym(p->code, tgToken(p).attrib.string);
      tgLexer_parse(p); /* AA ( ?? -> ( __ ?? */
      tgParser_call(p, off);
      break;
    default:
      assert(0);
      break;
  }
  tgLexer_parse(p); /* Eat ; */
}

static void tgParser_if(tgParser* p);
static void tgParser_while(tgParser* p);
static void tgParser_scoped(tgParser* p);

static void tgParser_stmt(tgParser* p) {
  switch (tgTag(p)) {
    case TG_IF:
      tgParser_if(p);
      break;
    case TG_WHILE:
      tgParser_while(p);
      break;
    case TG_LCURLEY:
      tgParser_scoped(p);
      break;
    default:
      tgParser_stmtBody(p);
      break;
    case TG_EOF:
      return;
  }
}

static void tgParser_scoped(tgParser* p) {
  tgLexer_parse(p);
  while (tgTag(p) != TG_RCURLEY) {
    tgParser_stmt(p);
  }
  tgLexer_parse(p);
}

static void tgParser_while(tgParser* p) {
  size_t preexpr, joff, eoff;
  tgLexer_expect(p, TG_LPARENS);
  tgLexer_parse(p);
  preexpr = p->code->curr - p->code->begin;
  tgParser_expr(p,0);
  tgLexer_parse(p);
  tgCode_jif(p->code);
  joff = p->code->curr - p->code->begin;
  tgParser_stmt(p);
  tgCode_rjmp(p->code);
  eoff = p->code->curr - p->code->begin;
  p->code->begin[joff - 1] = eoff - joff;
  p->code->begin[eoff - 1] = eoff - preexpr;
}

static void tgParser_if(tgParser* p) {
  size_t joff, eoff, jump;
  tgLexer_expect(p, TG_LPARENS);
  tgLexer_parse(p);
  tgParser_expr(p,0);
  tgLexer_parse(p);
  tgCode_jif(p->code);
  joff = p->code->curr - p->code->begin;
  tgParser_stmt(p);
  if (tgTag(p) == TG_ELSE)
    tgCode_jmp(p->code);
  eoff = p->code->curr - p->code->begin;
  jump = eoff - joff;
  p->code->begin[joff - 1] = jump;
  if (tgTag(p) == TG_ELSE) {
    size_t eelse, jelse;
    tgLexer_parse(p);
    tgParser_stmt(p);
    eelse = p->code->curr - p->code->begin;
    jelse = eelse - eoff;
    p->code->begin[eoff - 1] = jelse;
  }
}

static void tgParser_scope(tgParser* p) {
  do {
    tgParser_stmt(p);
  } while (tgTag(p) != TG_EOF);
}

void tgParser_parse(tgParser* parser) {
  tgLexer_parse(parser);
  tgParser_scope(parser);
}
