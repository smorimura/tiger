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
tgLexer* tgLexer_alloc(tgState* T, tgBuffer* b) {
  tgLexer* lex = tgAlloc(sizeof(tgLexer));
  tgLexer_init(T, lex, b);
  return lex;
}

void tgLexer_init(tgState* T, tgLexer* lex, tgBuffer* b) {
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

void tgLexer_dinit(tgLexer* lex) {
}

void tgLexer_free(tgState* T, tgLexer* lex) {
  tgLexer_dinit(lex);
  tgFree(lex);
}

static void tgLexer_error(char const* fmt, ...) {
  va_list vargs;
  va_start(vargs, fmt);
  vprintf(fmt, vargs);
  va_end(vargs);
}

void tgLexer_next(tgLexer* lex) {
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

tgTag tgLexer_singleLineComment(tgLexer* lex) {
  do {
    tgLexer_next(lex);
  } while (tgTerm(lex) != '\n');
  ++lex->lcomment;
  return TG_NOP;
}

tgTag tgLexer_multiLineComment(tgLexer* lex) {
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

// Currently, this does nothing. Reserved for future use.
tgTag tgLexer_informationalComment(tgLexer* lex) {
  char prev;
  for (;;) {
    tgLexer_next(lex);
    if (prev == '!' && tgTerm(lex) == '/') break;
    if (tgTerm(lex) == '\n') ++lex->lcomment;
    prev = tgTerm(lex);
  }
  return TG_NOP;
}

tgTag tgLexer_preprocessor(tgLexer* lex) {
  tgLexer_next(lex);
  switch (tgTerm(lex)) {
    case '!':
      // Crunch-Bang symbol
      return tgLexer_singleLineComment(lex);
    default:
      tgLexer_error("Undefined preprocessor formation! Expected `!` found `%c`\n", tgTerm(lex));
      break;
  }
}

tgTag tgLexer_add(tgLexer* lex) {
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

tgTag tgLexer_sub(tgLexer* lex) {
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

tgTag tgLexer_mul(tgLexer* lex) {
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

tgTag tgLexer_divOrComment(tgLexer* lex) {
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
static tgLexer_checkKeyword(tgLexer* lex, char const* buff) {
  CHK(if, TG_IF);
  CHK(else, TG_ELSE);
  CHK(while, TG_WHILE);
  CHK(do, TG_DO);
  return TG_IDENTIFIER;
}
#undef CHK

tgTag tgLexer_identifier(tgLexer* lex) {
  char buff[4096];
  char *str = buff;
  do {
    *str = tgTerm(lex); ++str;
    tgLexer_next(lex);
  } while (isalpha(tgTerm(lex)) || isnum(tgTerm(lex)));
  *str = '\0';
  --lex->buffer->curr;

  tgTag t = tgLexer_checkKeyword(lex, buff);
  if (t != TG_IDENTIFIER) return t;

  tgToken(lex).type = TA_STRING;
  tgStringFromCharPtr(tgAttrib(lex).string, buff);
  return t;
}

tgTag tgLexer_real(tgLexer* lex, int whole) {
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

  tgToken(lex).type = TA_REAL;
  float real = whole + ((float)decimal) / digits;
  tgRealFromFloat(tgAttrib(lex).real, real);
  return TG_REAL;
}

tgTag tgLexer_integer(tgLexer* lex) {
  int whole = 0;
  do {
    whole = 10 * whole + tonum(tgTerm(lex));
    tgLexer_next(lex);
  } while (isnum(tgTerm(lex)));

  // Check for decimal
  if (tgTerm(lex) == '.') {
    tgLexer_next(lex);
    return tgLexer_real(lex, whole);
  }
  --lex->buffer->curr;

  tgToken(lex).type = TA_INTEGER;
  tgIntegerFromInt(tgAttrib(lex).integer, whole);
  return TG_INTEGER;
}

tgTag tgLexer_declaration(tgLexer* lex) {
  tgLexer_next(lex);
  switch (tgTerm(lex)) {
    case '=':
      return TG_DEFINE;
    default:
      return TG_DECLARE;
  }
}

tgTag tgLexer_assign(tgLexer* lex) {
  tgLexer_next(lex);
  switch (tgTerm(lex)) {
    case '=':
      return TG_EQ;
    default:
      return TG_ASSIGN;
  }
}

tgTag tgLexer_lt(tgLexer* lex) {
  tgLexer_next(lex);
  switch (tgTerm(lex)) {
    case '=':
      return TG_LE;
    default:
      return TG_LT;
  }
}

tgTag tgLexer_gt(tgLexer* lex) {
  tgLexer_next(lex);
  switch (tgTerm(lex)) {
    case '=':
      return TG_GE;
    default:
      return TG_GT;
  }
}

tgTag tgLexer_string(tgLexer* lex) {
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

tgTag tgLexer_parse_(tgLexer* lex) {
  int lookahead = 0;
  while (++lookahead < TIGER_TOKEN_COUNT) {
    lex->tokens[lookahead - 1] = lex->tokens[lookahead];
  }
  lex->tokens[TIGER_TOKEN_COUNT - 1].type = TA_NONE;
  tgTag tg;
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

tgTag tgLexer_parse(tgLexer* lex) {
  do {
    lex->tokens[TIGER_TOKEN_COUNT - 1].tag = tgLexer_parse_(lex);
  } while(lex->tokens[0].tag == TG_ERROR);
  return lex->tokens[0].tag;
}

void tgLexer_expect(tgLexer* lex, tgTag t) {
  if (tgLexer_parse(lex) != t)
    tgLexer_error("Expected a specific tag!\n");
}
