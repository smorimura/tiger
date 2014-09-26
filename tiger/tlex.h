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
  TG_ID, TG_REAL, TG_OPERATOR, TG_TERMINAL
} tgTag;

/*******************************************************************************
 * Type for holding token tags.
 ******************************************************************************/
typedef struct {
  tgTag tag;
} tgToken;

/*******************************************************************************
 * Type for holding real tokens.
 ******************************************************************************/
typedef struct {
  tgToken token;
  mpf_t value;
} tgTReal;

/*******************************************************************************
 * Type for holding identifier tokens.
 ******************************************************************************/
typedef struct {
  tgToken token;
  char *name;
} tgTId;

/*******************************************************************************
 * Type for holding operator tokens.
 ******************************************************************************/
typedef struct {
  tgToken token;
  char *string;
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
tgToken* tgLexer_scan(tgLexer* lexer, FILE* input);

#endif // TG_TLEX_H
