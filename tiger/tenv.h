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
  int (*callback)(struct tgState_*);
  tgTag tag;
};
typedef struct tgSymbol_ tgSymbol;

struct tgEnv_* tgEnv_alloc(struct tgState_* T, int size, struct tgEnv_* prev);
struct tgSymbol_* tgEnv_newSym(struct tgState_* T, struct tgEnv_* env, const char* name);
struct tgSymbol_* tgEnv_getSym(struct tgEnv_* env, char const* name);
struct tgSymbol_* tgEnv_getLocalSym(struct tgEnv_* env, char const* name);
void tgEnv_free(struct tgEnv_* env);

#endif /* TIGER_TENV_H */
