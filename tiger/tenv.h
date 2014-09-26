
#ifndef   TIGER_TENV_H
#define   TIGER_TENV_H

#include <stddef.h>
#include <tiger/tlex.h>

typedef struct tgSymbol_ {
  struct tgSymbol_ *next;
  size_t hash;
  tgTId *token;
} tgSymbol;

typedef struct tgEnv_ {
  int size;
  struct tgEnv_ *pEnv;
  tgSymbol **symbols;
} tgEnv;

tgEnv* tgEnv_create(int size, tgEnv* prev);
tgToken* tgEnv_get(tgEnv* env, char const* name);
void tgEnv_add(tgEnv* env, tgTId* token);

#endif // TIGER_TENV_H
