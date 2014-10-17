/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tstate.h>
#include <tiger/tenv.h>
#include <tiger/tlex.h>
#include <tiger/tparse.h>
#include <malloc.h>

struct tgBuffer_ {
  size_t bytes;
  char *buffer;
  void *data;
};
typedef struct tgBuffer_ tgBuffer;

void tgState_execBuffer(tgState* T, tgBuffer *buff) {
  do {
    
  } while (buff->read(buff));
}

tgState* tgState_alloc() {
  tgState* T = malloc(sizeof(tgState));
  T->env = tgEnv_alloc(5, NULL);
  T->lexer = tgLexer_alloc();
  T->parser = tgParser_alloc();
  return T;
}

int tgState_addCFunc(tgState* T, char const* fname, tgCallback func) {
  tgSymbol* sym = tgEnv_newSym(T->env, fname);

  if(!sym) return 0;

  sym->data = func;
  sym->tag = TG_FUNCTION;

  return 1;
}

void tgState_execStr(tgState* T, char const* str) {
  tgBuffer *buff = tgBuffer_allocStr(str);
  tgState_execBuffer(T, buff);
}

void tgState_execFile(tgState* T, char const* file) {
  tgBuffer *buff = tgBuffer_allocFile(file);
  tgState_execBuffer(T, buff);
}

void tgState_free(tgState* T) {
  tgEnv_free(T->env);
  tgLexer_free(T->lexer);
  tgParser_free(T->parser);
  free(T);
}
