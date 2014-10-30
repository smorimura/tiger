/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tenv.h>
#include <tiger/talloc.h>
#include <malloc.h>
#include <string.h>

/*******************************************************************************
 * Environment Definitions
 ******************************************************************************/
struct tgEnvSym_ {
  tgSymbol sym;
  struct tgEnvSym_ *next;
  size_t hash;
};
typedef struct tgEnvSym_ tgEnvSym;

struct tgEnv_ {
  int size;
  struct tgEnv_ *pEnv;
  tgEnvSym **symbols;
};
typedef struct tgEnv_ tgEnv;

/*******************************************************************************
 * Environment Symbol Private Methods
 ******************************************************************************/

/**
 * @brief Allocates a new instance of tgEnvSym with all of the properties
 *        initialized. This function does not concatenate the symbol with
 *        a parent symbol list, that must be done by the caller.
 *
 * @param name The name of the symbol.
 * @param namelen The length of the string \p name.
 * @param hash The hash directly relating to \p name.
 * @return A tgEnvSym that may be concatenated with an existing symbol list.
 */

static tgEnvSym* tgEnvSym_alloc(tgState* T, char const* name, size_t namelen, size_t hash) {
  tgEnvSym* sym = tgAlloc(sizeof(tgEnvSym));

  sym->hash = hash;
  sym->next = NULL;
  sym->sym.data = NULL;
  sym->sym.name = malloc(namelen + 1);
  sym->sym.tag = TG_ERROR;

  strcpy((char*)sym->sym.name, name);

  return sym;
}

/*******************************************************************************
 * Environment Private Methods
 ******************************************************************************/

/**
 * @brief Hashes the string \p c for hashmap lookups within tgEnv.
 *
 * @param c The string to be hashed.
 * @param n [out] Returns the size of the string.
 * @return The hash value correlating to the string \p c.
 */
static size_t tgEnv_hash_(char const *c, size_t *n) {
  size_t hash = 0;
  while(*c) {
    hash = hash * 255 + *c;
    ++c; ++*n;
  }
  return hash;
}

/**
 * @brief Hashes the string \p c for hashmap lookups within tgEnv.
 *
 * @param c The string to be hashed.
 * @return The hash value correlating to the string \p c.
 */
static size_t tgEnv_hash(char const *c) {
  static size_t n;
  return tgEnv_hash_(c, &n);
}

/**
 * @brief Searches through the environment \p env for a symbol matching the name
 *        \p name and the hash \p hash.
 *
 * @param env The tgEnv to search through.
 * @param name The name of the symbol to search for.
 * @param hash The hash of the string \p name.
 * @return A symbol matching the name \p name and the hash \p hash, or NULL if
 *         no symbol could be found.
 */
static tgEnvSym* tgEnv_getLocalSym_(tgEnv* env, char const* name, size_t hash) {
  size_t idx = hash % env->size;
  tgEnvSym* sym = env->symbols[idx];

  // Search through all possible matches for the symbol.
  while(sym) {
    if(sym->hash == hash && (strcmp(sym->sym.name, name) == 0))
      return sym;
    else
      sym = sym->next;
  }

  return NULL;
}

/**
 * @brief Searches all of the tgEnv starting from \p env and travelling up the
 *        parents in search of a symbol named \p name with a hash \p hash.
 *
 * @param env The environment to search through.
 * @param name The name of the symbol to search for.
 * @param hash The precalculated hash value for the string \p name.
 * @return Either the symbol returned by matching \p name and \p hash, or NULL
 *         if the symbol does not exist.
 */
static tgEnvSym* tgEnv_getSym_(tgEnv* env, char const* name, size_t hash) {
  size_t idx;
  tgEnvSym* sym;

  // Search through all parent environments for the symbol.
  while(env) {
    sym = tgEnv_getLocalSym_(env, name, hash);
    if(sym) return sym;
    else env = env->pEnv;
  }

  return NULL;
}

/*******************************************************************************
 * Environment Public Methods
 ******************************************************************************/

/**
 * @brief Allocates a new instance of tgEnv which is scoped within another
 *        tgEnv
 *
 * @param size Size of the hashmap within the tgEnv.
 * @param prev A pointer to the parent tgEnv (may be NULL for no parent).
 * @return An instance of tgEnv which is nested within \p prev.
 */
tgEnv* tgEnv_alloc(tgState* T, int size, tgEnv* prev) {
  tgEnv* env = tgAlloc(sizeof(tgEnv));

  env->size = size;
  env->pEnv = prev;
  env->symbols = tgAlloc(size * sizeof(tgEnvSym*));

  return env;
}

/**
 * @brief Creates a new blank symbol in the top-most tgEnv.
 *
 * @param env The tgEnv to create the symbol within.
 * @param name The name of the symbol - used for lookup table.
 * @return The newly constructed symbol.
 */
tgSymbol* tgEnv_newSym(tgState* T, tgEnv* env, const char* name) {
  size_t symlen = 0;
  size_t hash = tgEnv_hash_(name, &symlen);
  size_t idx = hash % env->size;
  tgEnvSym* symbol = tgEnv_getLocalSym_(env, name, hash);

  if (symbol) return NULL;

  // Symbol does not exist, create it
  symbol = tgEnvSym_alloc(T, name, symlen, hash);
  symbol->next = env->symbols[idx];
  env->symbols[idx] = symbol;

  return (tgSymbol*)symbol;
}

/**
 * @brief Gets a symbol from the tgEnv table. This is an interative search of
 *        the current tgEnv, and all parent tgEnv that \p env is scoped within.
 *
 * @param env The target tgEnv in which to search through.
 * @param name The name of the symbol to search for.
 * @return Either the found symbol, or NULL if the current tgEnv \p env and none
 *         of the parent tgEnv's carry a symbol named \p name.
 */
tgSymbol* tgEnv_getSym(tgEnv* env, char const* name) {
  size_t hash = tgEnv_hash(name);
  return (tgSymbol*)tgEnv_getSym_(env, name, hash);
}

/**
 * @brief Gets a symbol from the tgEnv table. This is a local search which does
 *        not take any of the parent tgEnvs within \p env into consideration.
 *
 * @param env The tgEnv to search through.
 * @param name The name of the symbol to search for.
 * @return Either the found symbol, or NULL if the current tgEnv \p env does not
 *         contain a symbol named \p name.
 */
tgSymbol* tgEnv_getLocalSym(tgEnv* env, char const* name) {
  size_t hash = tgEnv_hash(name);
  return (tgSymbol*)tgEnv_getLocalSym_(env, name, hash);
}

/**
 * @brief Frees \p env tgEnv properly.
 *
 * @param env The tgEnv that should be freed.
 * @return void
 */
void tgEnv_free(tgEnv* env) {
  free(env);
}
