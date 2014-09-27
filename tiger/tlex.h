/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TG_TLEX_H
#define   TG_TLEX_H

#include <tiger/talloc.h>
#include <gmp.h>
#include <stdio.h>

/*******************************************************************************
 * Definitions for the different token values.
 ******************************************************************************/
typedef enum {
  TG_ID, TG_REAL, TG_OPERATOR, TG_TERMINAL, TG_FUNCTION
} tgTag;

/*******************************************************************************
 * Type for holding token tags.
 ******************************************************************************/
typedef struct {
  tgTag tag;
} tgToken;

/*******************************************************************************
 * Type for holding identifier tokens.
 ******************************************************************************/
typedef struct {
  tgToken token;
  char *name;
} tgTId;

/*******************************************************************************
 * Type for holding real tokens.
 ******************************************************************************/
typedef struct {
  tgTId id;
} tgTReal;

/*******************************************************************************
 * Type for holding operator tokens.
 ******************************************************************************/
typedef struct {
  tgTId id;
} tgTOperator;

/*******************************************************************************
 * Type for holding a single nonterminal
 ******************************************************************************/
typedef struct {
  tgToken token;
  char nonterm;
} tgTTerm;

/*******************************************************************************
 * tgLexer class
 ******************************************************************************/
typedef struct {
} tgLexer;
extern tgClass tgLexer_class;

// Functions associated with tgLexer
struct tgEnv_;
tgToken* tgLexer_scan(struct tgEnv_* env, tgLexer* lexer, FILE* input);
tgToken* tgTId_create(char const* buffer);

#endif // TG_TLEX_H
