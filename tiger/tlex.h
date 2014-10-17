/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TG_TLEX_H
#define   TG_TLEX_H

/*******************************************************************************
 * Definitions for the different token values.
 ******************************************************************************/
enum tgTag_ {
  TG_UNDEFINED, TG_TAG, TG_ID, TG_REAL, TG_OPERATOR, TG_TERMINAL, TG_FUNCTION
};
typedef enum tgTag_ tgTag;

/*******************************************************************************
 * The structure for the lexical analyzer
 ******************************************************************************/
struct tgLexer_ {
};
typedef struct tgLexer_ tgLexer;

/*******************************************************************************
 * tgLexer Public Methods
 ******************************************************************************/
struct tgEnv_;

tgLexer* tgLexer_alloc();
void tgLexer_free(tgLexer* lex);

#endif // TG_TLEX_H
