/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TIGER_TSTATE_H
#define   TIGER_TSTATE_H

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/
struct tgEnv_;
struct tgParser_;
struct tgLexer_;

/*******************************************************************************
 * State Declaration
 ******************************************************************************/
struct tgState_ {
  struct tgEnv_ *env;
  struct tgParser_ *parser;
  struct tgLexer_ *lexer;
};
typedef struct tgState_ tgState;
typedef int(*tgCallback)(tgState*);

/*******************************************************************************
 * State Methods
 ******************************************************************************/
tgState* tgState_alloc();
int tgState_addCFunc(tgState* T, const char* fname, tgCallback func);
void tgState_execStr(tgState* T, char const* str);
void tgState_free(tgState* T);

#endif // TIGER_TSTATE_H
