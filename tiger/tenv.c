
#include <tiger/tenv.h>
#include <malloc.h>

static tgSymbol* tgSymbol_create(tgTId* token, size_t hash, void* data, tgSymbol* next) {
  tgSymbol* sym = malloc(sizeof(tgSymbol));
  sym->hash = hash;
  sym->next = next;
  sym->token = token;
  sym->data = data;
}

static size_t tgEnv_hash(char const* c) {
  size_t hash = 0;
  while(*c) {
    hash = hash * 255 + *c;
    ++c;
  }
  return hash;
}

tgEnv* tgEnv_create(int size, tgEnv* prev) {
  size_t i;
  tgEnv* env = malloc(sizeof(tgEnv));

  env->size = size;
  env->pEnv = prev;
  env->symbols = malloc(sizeof(tgTId*)*size);
  for(i = 0; i < size; ++i) {
    env->symbols[i] = NULL;
  }

  return env;
}

static tgSymbol* tgEnv_get_immediate_(tgEnv* env, char const* name, size_t hash) {
  size_t idx = hash % env->size;
  tgSymbol* sym = env->symbols[idx];

  // Search each environment for symbols
  while(sym) {
    if(sym->hash == hash && (strcmp(sym->token->name, name) == 0))
      return sym;
    else
      sym = sym->next;
  }

  return NULL;
}

static tgSymbol* tgEnv_get_(tgEnv* env, char const* name, size_t hash) {
  size_t idx;
  tgSymbol* sym;

  // Move through environments
  while(env) {
    sym = tgEnv_get_immediate_(env, name, hash);
    if(sym) return sym;
    else env = env->pEnv;
  }

  return NULL;
}

tgSymbol* tgEnv_getSym(tgEnv* env, char const* name) {
  size_t hash = tgEnv_hash(name);
  return tgEnv_get_(env, name, hash);
}

tgToken* tgEnv_get(tgEnv* env, char const* name) {
  tgSymbol* sym = tgEnv_getSym(env, name);

  if(sym) return (tgToken*)sym->token;
  else return NULL;
}

void tgEnv_add(tgEnv* env, tgTId* token, void* data) {
  size_t hash = tgEnv_hash(token->name);
  size_t idx = hash % env->size;
  tgSymbol* next = env->symbols[idx];

  env->symbols[idx] = tgSymbol_create(token, hash, data, next);
}
