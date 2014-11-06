/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/talloc.h>
#include <tiger/tstate.h>
#include <malloc.h>

void* tgMalloc(void* ud, void* ptr, size_t req) {
  (void)ud;
  return realloc(ptr, req);
}

void* tgAlloc_(struct tgState_* T, size_t s) {
  return T->allocator(T->allocatorData, NULL, s);
}

void* tgAllocChk_(struct tgState_* T, size_t s, char const* file, size_t line) {
  (void)file;
  (void)line;
  return tgAlloc_(T, s);
}

void  tgFree_(struct tgState_* T, void* ptr) {
  (void)T->allocator(T->allocatorData, ptr, 0);
}

void  tgFreeChk_(struct tgState_* T, void* ptr, char const* file, size_t line) {
  (void)file;
  (void)line;
  tgFree_(T, ptr);
}
