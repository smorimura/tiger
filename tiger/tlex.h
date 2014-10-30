/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TG_TLEX_H
#define   TG_TLEX_H

#include <tiger/fwdtiger.h> /* tgState, tgBuffer */
#include <tiger/tdef.h>
#include <tiger/tconfig.h>
#include <stddef.h> /* size_t */

/*******************************************************************************
 * Definitions for the different token values.
 ******************************************************************************/
enum tgTag_ {
  TG_ERROR, TG_EOF, TG_ENDSTMT, TG_LPARENS, TG_RPARENS, TG_LCURLEY, TG_RCURLEY,
  // Raw Types
  TG_NOP, TG_IDENTIFIER, TG_CFUNCTION,
  // Branching/Looping
  TG_IF, TG_ELSE, TG_WHILE, TG_DO, TG_ELSEIF, TG_FOR,
  // Arithmetic
  TG_ADD, TG_ADDEQUAL, TG_SUB, TG_SUBEQUAL, TG_MUL, TG_MULEQUAL, TG_DIV, TG_DIVEQUAL,
  // Literals
  TG_INTEGER, TG_REAL, TG_STRING,
  // Specialized Operators
  TG_DECLARE, TG_DEFINE,
  // Relational
  TG_EQ, TG_LT, TG_LE, TG_GT, TG_GE, TG_ASSIGN
};
typedef enum tgTag_ tgTag;

union tgAttrib_ {
  tgReal real;
  tgInteger integer;
  tgString string;
};
typedef union tgAttrib_ tgAttrib;

enum tgAttribType_ {
  TA_INVALID,
  TA_NONE,
  TA_REAL,
  TA_INTEGER,
  TA_STRING
};
typedef enum tgAttribType_ tgAttribType;

struct tgToken_ {
  tgTag tag;
  tgAttrib attrib;
  tgAttribType type;
};
typedef struct tgToken_ tgToken;
/*******************************************************************************
 * The structure for the lexical analyzer
 ******************************************************************************/
struct tgLexer_ {
  size_t lchar;
  size_t lline;
  size_t lcomment;
  char terminal;
  tgToken tokens[TIGER_TOKEN_COUNT];
  tgBuffer *buffer;
  char const* filename;
  tgCode* code;
};
typedef struct tgLexer_ tgLexer;

/*******************************************************************************
 * tgLexer Public Methods
 ******************************************************************************/

tgLexer* tgLexer_alloc(tgState* T, tgBuffer* b);
void tgLexer_init(tgState* T, tgLexer* lex, tgBuffer* b);
void tgLexer_dinit(tgLexer* lex);
void tgLexer_free(tgState* T, tgLexer* lex);

tgTag tgLexer_parse(tgLexer* lex);
void tgLexer_expect(tgLexer* lex, tgTag t);

#endif // TG_TLEX_H
