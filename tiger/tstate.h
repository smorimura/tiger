/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TIGER_TSTATE_H
#define   TIGER_TSTATE_H

#include <tiger/fwdtiger.h>
#include <tiger/talloc.h>
#include <tiger/tvalue.h>
#include <tiger/tcode.h>

/*******************************************************************************
 * State Declaration
 ******************************************************************************/
struct tgState_ {
  struct tgEnv_ *env;
  tgValue *stack;
  tgValue *top;
  tgValue *ktab;
  tgValue *ktop;
  tgAllocator allocator;
  void *allocatorData;
};
typedef struct tgState_ tgState;
typedef int(*tgCallback)(tgState*);

/*******************************************************************************
 * State Methods
 ******************************************************************************/
tgState* tgState_create(tgAllocator alloc, void* ud);
void tgState_free(tgState* T);

int tgState_addCFunc(tgState* T, const char* fname, tgCallback func);

/* State Manipulation */
tgValue* tgState_defK(tgState* T, tgValue v);
tgValue* tgState_defS(tgState* T, tgValue* kn);
void tgState_push(tgState* T, tgValue* v);
void tgState_pushk(tgState* T, tgValue* v);
void tgState_pushInt(tgState* T, tgInteger k);
void tgState_pushStr(tgState* T, tgString str);
void tgState_pushPtr(tgState* T, void* k);
void tgState_call(tgState* T, tgCallback cb);
tgValue* tgState_pop(tgState* T);
void tgState_callb(tgState* T, tgByteCode c);

/* Code Execution / Compilation */
void tgState_execFile(tgState* T, char const* path);
void tgState_execLiteral(tgState* T, char const* str);
void tgState_execCode(tgState* T, tgCode* c);
tgCode* tgState_compileFile(tgState* T, char const* path);
tgCode* tgState_compileLiteral(tgState* T, char const* str);

#endif /* TIGER_TSTATE_H */
