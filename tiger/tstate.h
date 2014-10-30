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
  tgEnv *env;
  tgValue *stack;
  tgValue rax;
  tgAllocator allocator;
  void *allocatorData;
};
typedef struct tgState_ tgState;
typedef int(*tgCallback)(tgState*);

/*******************************************************************************
 * State Methods
 ******************************************************************************/
tgState* tgState_create(tgAllocator alloc, void* ud);
int tgState_addCFunc(tgState* T, const char* fname, tgCallback func);
void tgState_execFile(tgState* T, char const* path);
void tgState_execLiteral(tgState* T, char const* str);
tgCode* tgState_compileFile(tgState* T, char const* path);
void tgState_compileLiteral(tgState* T, char const* str);
void tgState_free(tgState* T);

#endif // TIGER_TSTATE_H
