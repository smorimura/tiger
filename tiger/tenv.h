
#ifndef   TIGER_TENV_H
#define   TIGER_TENV_H

#include <stddef.h>
#include <tiger/tlex.h>

typedef struct tgSymbol_ {
  struct tgSymbol_ *next;
  size_t hash;
  tgTId *token;
  void* data;
} tgSymbol;

typedef struct tgEnv_ {
  int size;
  struct tgEnv_ *pEnv;
  tgSymbol **symbols;
} tgEnv;

tgEnv* tgEnv_create(int size, tgEnv* prev);
tgSymbol* tgEnv_getSym(tgEnv* env, char const* name);
tgToken* tgEnv_get(tgEnv* env, char const* name);
void tgEnv_add(tgEnv* env, tgTId* token, void* data);

#endif // TIGER_TENV_H
