/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tlex.h>
#include <string.h>
#include <malloc.h>

tgClass tgLexer_class = {
  sizeof(tgLexer), &tgNull_ctor, &tgNull_dtor
};

static int peek = 0;
static int lines = 0;
static char buffer[4086];

static void tgLexer_read(FILE* input) {
  peek = fgetc(input);
}

static int issymbol(int c) {
  switch(c) {
    case '!':
    case '@':
    case '#':
    case '$':
    case '%':
    case '^':
    case '&':
    case '*':
    case '<':
    case '>':
    case '?':
    case '/':
    case '-':
    case '+':
    case '=':
      return 1;
    default:
      break;
  }
  return 0;
}

static int isnum(int c) {
  return (c >= '0' && c <= '9');
}

static int isalphanum(int c) {
  return (isalpha(c) || isnum(c));
}

static tgToken* tgToken_create(char c) {
  tgToken* token = malloc(sizeof(tgToken));
  token->tag = TG_ID;
  return token;
}

static tgToken* tgTTerm_create(char c) {
  tgTTerm* term = malloc(sizeof(tgTTerm));
  term->token.tag = TG_TERMINAL;
  term->nonterm = c;
}

static tgToken* tgTReal_create(char const* buffer) {
  tgTReal *real = malloc(sizeof(tgTReal));
  real->token.tag = TG_REAL;
  mpf_init_set_str(real->value, buffer, 10);
  return (tgToken*)real;
}

static tgToken* tgTId_create(char const* buffer) {
  tgTId *id = malloc(sizeof(tgTId));
  id->token.tag = TG_ID;
  id->name = malloc(sizeof(char) * strlen(buffer) + 1);
  strcpy(id->name, buffer);
  return (tgToken*)id;
}

static tgToken* tgTOperator_create(char const* buffer) {
  tgTOperator *id = malloc(sizeof(tgTOperator));
  id->token.tag = TG_OPERATOR;
  id->string = malloc(sizeof(char) * strlen(buffer) + 1);
  strcpy(id->string, buffer);
  return (tgToken*)id;
}

tgToken* tgLexer_scan(tgLexer* lexer, FILE* input) {
  // Skip whitespaces
  for(;;) {
    tgLexer_read(input);
    if(peek == ' ' || peek == '\t') continue;
    else if(peek == '\n') ++lines;
    else break;
  }

  // Reals
  if(isnum(peek)) {
    char* curr = buffer;
    do {
      *curr = peek;
      tgLexer_read(input);
      ++curr;
    } while(isnum(peek));
    if(peek != '.') (*curr) = '\0';
    else do {
      *curr = peek;
      tgLexer_read(input);
      ++curr;
    } while(isnum(peek));
    (*curr) = '\0';
    return tgTReal_create(buffer);
  }

  // Identifier
  if(isalpha(peek)) {
    char* curr = buffer;
    do {
      *curr = peek;
      tgLexer_read(input);
      ++curr;
    } while(isalphanum(peek));
    (*curr) = '\0';
    // tgTId const* token = tgSym_get(buffer);
    // if(token != NULL) return token;
    tgToken* token = tgTId_create(buffer);
    // tgSym_reg(token);
    return token;
  }

  // Operator
  if(issymbol(peek)) {
    char* curr = buffer;
    do {
      *curr = peek;
      tgLexer_read(input);
      ++curr;
    } while(issymbol(peek));
    (*curr) = '\0';
    tgToken* token = tgTOperator_create(buffer);
    return token;
  }

  // Misc. Characters
  tgToken *token = tgTTerm_create(peek);
  return token;
}
