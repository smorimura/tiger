/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tstate.h>
#include <tiger/tenv.h>
#include <tiger/tlex.h>
#include <tiger/tparse.h>
#include <tiger/tbuffer.h>
#include <malloc.h>

/*******************************************************************************
 * tgState Private Methods
 ******************************************************************************/

static void tgState_execBuffer(tgState* T, tgBuffer* buffer) {
  tgLexer parser;
  tgLexer_init(T, &parser, buffer);
  do {
    tgParser_parse(&parser);
  } while (parser.tokens[0].tag != TG_EOF);
  tgLexer_dinit(&parser);
}

static tgCode* tgState_compileBuffer(tgState* T, tgBuffer* buffer) {
  tgLexer parser;
  tgCode *finalCode;

  tgLexer_init(T, &parser, buffer);
  do {
    tgParser_parse(&parser);
  } while (parser.tokens[0].tag != TG_EOF);
  finalCode = parser.code;
  tgLexer_dinit(&parser);

  return finalCode;
}

/*******************************************************************************
 * tgState Public Methods
 ******************************************************************************/

/**
 * @brief Creates a new tgState with no symbols, constants, or bytecode.
 *
 * @param alloc The allocator to use within this state.
 * @param ud The userdata that will be passed to \p alloc.
 * @return tgState*
 */
tgState* tgState_create(tgAllocator alloc, void* ud) {
  tgState* T = alloc(ud, NULL, sizeof(tgState));
  T->allocator = alloc;
  T->allocatorData = ud;
  T->env = tgEnv_alloc(T, 5, NULL);
  return T;
}

/**
 * @brief Frees a state and all symbols, constants, and bytecode associated.
 *
 * @param T The state that should be freed.
 * @return void
 */
void tgState_free(tgState* T) {
  tgEnv_free(T->env);
  free(T);
}

int tgState_addCFunc(tgState* T, char const* fname, tgCallback func) {
  tgSymbol* sym = tgEnv_newSym(T, T->env, fname);

  if(!sym) return 0;

  sym->data = func;
  sym->tag = TG_CFUNCTION;

  return 1;
}

void tgState_execFile(tgState* T, char const* path) {
  tgBuffer *b = tgBuffer_allocFile(T, path);
  tgState_execBuffer(T, b);
  tgBuffer_freeFile(T, b);
}

void tgState_execLiteral(tgState* T, char const* str) {
  tgBuffer *b = tgBuffer_allocLiteral(T, str);
  tgState_execBuffer(T, b);
  tgBuffer_freeLiteral(T, b);
}

void tgState_execBinary(tgState* T, char const* path) {
  tgBuffer *b = tgBuffer_allocFile(T, path);
  tgBuffer_freeFile(T, b);
}

tgCode* tgState_compileFile(tgState* T, char const* path) {
  tgCode *c = NULL;
  tgBuffer *b = tgBuffer_allocFile(T, path);
  if (b) {
    c = tgState_compileBuffer(T, b);
    tgBuffer_freeFile(T, b);
  }
  return c;
}

void tgState_compileLiteral(tgState* T, char const* str) {
}

static void tgPrintAttrib(tgToken* t) {
  switch (t->type) {
    case TA_INTEGER:
      printf("<%d>", t->attrib.integer);
      break;
    case TA_REAL:
      printf("<%f>", t->attrib.real);
      break;
    case TA_STRING:
      printf("<%s>", t->attrib.string);
      break;
  }
}
