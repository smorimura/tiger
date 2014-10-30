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

// Memory allocation routines
void* tgAlloc_(tgState* T, size_t s) {
  return T->allocator(T->allocatorData, NULL, s);
}

void* tgAllocChk_(tgState* T, size_t s, char const* file, size_t line) {
  return tgAlloc_(T, s);
}

void  tgFree_(tgState* T, void* ptr) {
  (void)T->allocator(T->allocatorData, ptr, 0);
}

void  tgFreeChk_(tgState* T, void* ptr, char const* file, size_t line) {
  tgFree_(T, ptr);
}
