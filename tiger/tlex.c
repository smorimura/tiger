/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tlex.h>
#include <tiger/talloc.h>
#include <tiger/tbuffer.h>
#include <tiger/tcode.h>
#include <malloc.h>
#include <stdarg.h>
#include <ctype.h>

#define tgTermN(l,n)  l->terminal
#define tgTerm(l)  tgTermN(l,0)
#define tgToken(l)  l->tokens[TIGER_TOKEN_COUNT - 1]
#define tgAttrib(l) tgToken(l).attrib
#define tonum(c) ((unsigned)((c) - '0'))
#define isnum(c) (tonum(c) < 10)

/*******************************************************************************
 * tgLexer Public Methods
 ******************************************************************************/
tgLexer* tgLexer_alloc(struct tgState_* T, struct tgBuffer_* b) {
  tgLexer* lex = tgAlloc(sizeof(tgLexer));
  tgLexer_init(T, lex, b);
  return lex;
}

void tgLexer_init(struct tgState_* T, struct tgLexer_* lex, struct tgBuffer_* b) {
  int lookahead = 0;
  lex->buffer = b;
  lex->filename = NULL;
  lex->lchar = 1;
  lex->lline = 1;
  lex->lcomment = 0;
  lex->code = tgCode_alloc(T);
  while (lookahead < TIGER_TOKEN_COUNT) {
    lex->tokens[lookahead++].tag = TG_ERROR;
  }
}

void tgLexer_dinit(struct tgLexer_* lex) {
  (void)lex;
}

void tgLexer_free(struct tgState_* T, struct tgLexer_* lex) {
  tgLexer_dinit(lex);
  tgFree(lex);
}

static void tgLexer_error(char const* fmt, ...) {
  va_list vargs;
  va_start(vargs, fmt);
  vprintf(fmt, vargs);
  va_end(vargs);
}

void tgLexer_next(struct tgLexer_* lex) {
  tgTerm(lex) = tgBuffer_next(lex->buffer);
  switch (tgTerm(lex)) {
    case '\n':
      ++lex->lline;
      lex->lchar = 0;
      break;
    default:
      ++lex->lchar;
      break;
  }
}

tgTag tgLexer_singleLineComment(struct tgLexer_* lex) {
  do {
    tgLexer_next(lex);
  } while (tgTerm(lex) != '\n' && tgTerm(lex) != EOF);
  ++lex->lcomment;
  return TG_NOP;
}

tgTag tgLexer_multiLineComment(struct tgLexer_* lex) {
  char last;
  size_t commentScope = 1;
  for (;;) {
    tgLexer_next(lex);
    switch (tgTerm(lex)) {
      case '/':
        if (last == '*')
          --commentScope;
        break;
      case '*':
        if (last == '/')
          ++commentScope;
        break;
      case '\n':
        ++lex->lcomment;
        break;
      default:
        break;
    }
    if (commentScope == 0)
      break;
    last = tgTerm(lex);
  }
  return TG_NOP;
}

tgTag tgLexer_informationalComment(struct tgLexer_* lex) {
  char prev;
  for (;;) {
    tgLexer_next(lex);
    if (prev == '!' && tgTerm(lex) == '/') break;
    if (tgTerm(lex) == '\n') ++lex->lcomment;
    prev = tgTerm(lex);
  }
  return TG_NOP;
}

tgTag tgLexer_preprocessor(struct tgLexer_* lex) {
  tgLexer_next(lex);
  switch (tgTerm(lex)) {
    case '!':
      return tgLexer_singleLineComment(lex);
    default:
      tgLexer_error("Undefined preprocessor formation! Expected `!` found `%c`\n", tgTerm(lex));
      break;
  }
  return TG_ERROR;
}

tgTag tgLexer_add(struct tgLexer_* lex) {
  tgLexer_next(lex);
  switch (tgTerm(lex)) {
    case '=':
      return TG_ADDEQUAL;
      break;
    default:
      return TG_ADD;
      break;
  }
}

tgTag tgLexer_sub(struct tgLexer_* lex) {
  tgLexer_next(lex);
  switch (tgTerm(lex)) {
    case '=':
      return TG_SUBEQUAL;
      break;
    default:
      return TG_SUB;
      break;
  }
}

tgTag tgLexer_mul(struct tgLexer_* lex) {
  tgLexer_next(lex);
  switch (tgTerm(lex)) {
    case '=':
      return TG_MULEQUAL;
      break;
    default:
      return TG_MUL;
      break;
  }
}

tgTag tgLexer_divOrComment(struct tgLexer_* lex) {
  tgLexer_next(lex);
  switch (tgTerm(lex)) {
    case '/':
      return tgLexer_singleLineComment(lex);
    case '*':
      return tgLexer_multiLineComment(lex);
    case '!':
      return tgLexer_informationalComment(lex);
    case '=':
      return TG_DIVEQUAL;
    default:
      return TG_DIV;
  }
}

#define CHK(k,v) if (strcmp(#k, buff) == 0) return v;
static int tgLexer_checkKeyword(struct tgLexer_* lex, char const* buff) {
  (void)lex;
  CHK(if, TG_IF);
  CHK(else, TG_ELSE);
  CHK(while, TG_WHILE);
  CHK(do, TG_DO);
  return TG_IDENTIFIER;
}
#undef CHK

tgTag tgLexer_identifier(struct tgLexer_* lex) {
  char buff[4096];
  char *str = buff;
  do {
    *str = tgTerm(lex); ++str;
    tgLexer_next(lex);
  } while (isalpha(tgTerm(lex)) || isnum(tgTerm(lex)));
  *str = '\0';
  --lex->buffer->curr;

  {
  tgTag t = tgLexer_checkKeyword(lex, buff);
  if (t != TG_IDENTIFIER) return t;

  tgToken(lex).type = TA_STRING;
  tgStringFromCharPtr(tgAttrib(lex).string, buff);
  return t;
  }
}

tgTag tgLexer_real(struct tgLexer_* lex, int whole) {
  size_t digits = 1;
  size_t decimal = 0;
  do {
    digits *= 10;
    decimal = 10 * decimal + tonum(tgTerm(lex));
    tgLexer_next(lex);
    if (digits >= 10000000) {
      while(isnum(tgTerm(lex))) tgLexer_next(lex);
    }
  } while (isnum(tgTerm(lex)));
  --lex->buffer->curr;

  {
    float real = whole + ((float)decimal) / digits;
    tgToken(lex).type = TA_REAL;
    tgRealFromFloat(tgAttrib(lex).real, real);
    return TG_REAL;
  }
}

tgTag tgLexer_integer(struct tgLexer_* lex) {
  int whole = 0;
  do {
    whole = 10 * whole + tonum(tgTerm(lex));
    tgLexer_next(lex);
  } while (isnum(tgTerm(lex)));

  /* Check for decimal */
  if (tgTerm(lex) == '.') {
    tgLexer_next(lex);
    return tgLexer_real(lex, whole);
  }
  --lex->buffer->curr;

  tgToken(lex).type = TA_INTEGER;
  tgIntegerFromInt(tgAttrib(lex).integer, whole);
  return TG_INTEGER;
}

tgTag tgLexer_declaration(struct tgLexer_* lex) {
  tgLexer_next(lex);
  switch (tgTerm(lex)) {
    case '=':
      return TG_DEFINE;
    default:
      return TG_DECLARE;
  }
}

tgTag tgLexer_assign(struct tgLexer_* lex) {
  tgLexer_next(lex);
  switch (tgTerm(lex)) {
    case '=':
      return TG_EQ;
    default:
      return TG_ASSIGN;
  }
}

tgTag tgLexer_lt(struct tgLexer_* lex) {
  tgLexer_next(lex);
  switch (tgTerm(lex)) {
    case '=':
      return TG_LE;
    default:
      return TG_LT;
  }
}

tgTag tgLexer_gt(struct tgLexer_* lex) {
  tgLexer_next(lex);
  switch (tgTerm(lex)) {
    case '=':
      return TG_GE;
    default:
      return TG_GT;
  }
}

tgTag tgLexer_string(struct tgLexer_* lex) {
  char buff[4096];
  char *str = buff;

  tgLexer_next(lex);
  do {
    *str = tgTerm(lex); ++str;
    tgLexer_next(lex);
  } while (tgTerm(lex) != '"');
  *str = '\0';

  tgToken(lex).type = TA_STRING;
  tgStringFromCharPtr(tgAttrib(lex).string, buff);
  return TG_STRING;
}

tgTag tgLexer_parse_(struct tgLexer_* lex) {
  tgTag tg;
  int lookahead = 0;
  while (++lookahead < TIGER_TOKEN_COUNT) {
    lex->tokens[lookahead - 1] = lex->tokens[lookahead];
  }
  lex->tokens[TIGER_TOKEN_COUNT - 1].type = TA_NONE;
  for (;;) {
    tgLexer_next(lex);
    switch (tgTerm(lex)) {
      case EOF:
        return TG_EOF;
        break;
      case '\n':case ' ':case '\r':case '\t':case '\f':case '\v':
        break;
      case '/':
        tg = tgLexer_divOrComment(lex);
        if (tg != TG_NOP) return tg;
        break;
      case '#':
        tgLexer_preprocessor(lex);
        break;
      case '+':
        return tgLexer_add(lex);
      case '-':
        return tgLexer_sub(lex);
      case '*':
        return tgLexer_mul(lex);
      case ':':
        return tgLexer_declaration(lex);
      case '{':
        return TG_LCURLEY;
      case '}':
        return TG_RCURLEY;
      case ';':
        return TG_ENDSTMT;
      case '(':
        return TG_LPARENS;
      case ')':
        return TG_RPARENS;
      case '=':
        return tgLexer_assign(lex);
      case '<':
        return tgLexer_lt(lex);
      case '>':
        return tgLexer_gt(lex);
      case '"':
        return tgLexer_string(lex);
      default:
        if (isalpha(tgTerm(lex))) {
          return tgLexer_identifier(lex);
        } else if (isnum(tgTerm(lex))) {
          return tgLexer_integer(lex);
        } else {
          tgLexer_error("Unexpected character!\n");
        }
    }
  }
}

tgTag tgLexer_parse(struct tgLexer_* lex) {
  do {
    lex->tokens[TIGER_TOKEN_COUNT - 1].tag = tgLexer_parse_(lex);
  } while(lex->tokens[0].tag == TG_ERROR);
  return lex->tokens[0].tag;
}

void tgLexer_expect(struct tgLexer_* lex, tgTag t) {
  if (tgLexer_parse(lex) != t)
    tgLexer_error("Expected a specific tag!\n");
}
