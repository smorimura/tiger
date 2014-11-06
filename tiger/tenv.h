/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TIGER_TENV_H
#define   TIGER_TENV_H

#include <stddef.h>
#include <tiger/fwdtiger.h> /* tgState */
#include <tiger/tlex.h>

/*******************************************************************************
 * Symbol Structure
 ******************************************************************************/

/**
 * @brief A registered symbol which can be found via tgEnv lookup.
 */
struct tgSymbol_ {
  char const *name;
  void *data;
  int (*callback)(tgState*);
  tgTag tag;
};
typedef struct tgSymbol_ tgSymbol;

tgEnv* tgEnv_alloc(tgState* T, int size, tgEnv* prev);
struct tgSymbol_* tgEnv_newSym(tgState* T, tgEnv* env, const char* name);
struct tgSymbol_* tgEnv_getSym(tgEnv* env, char const* name);
struct tgSymbol_* tgEnv_getLocalSym(tgEnv* env, char const* name);
void tgEnv_free(tgEnv* env);

#endif /* TIGER_TENV_H */
