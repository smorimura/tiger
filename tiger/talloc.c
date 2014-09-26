/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/talloc.h>
#include <malloc.h>

void* tgMalloc(tgClass const* tclass, size_t bytes) {
  bytes += sizeof(tgClass*);
  void* ptr = malloc(bytes);
  *((tgClass const**)ptr) = tclass;
  return ptr + sizeof(tgClass*);
}

void* tgAlloc_(tgClass const* tclass) {
  void* inst = tgMalloc(tclass, tclass->size);
  return tclass->ctor(inst);
}

void  tgFree(void* inst) {
  tgClass const* tclass = *(tgClass const**)(inst - sizeof(tgClass*));
  tclass->dtor(inst);
  free(inst - sizeof(tgClass*));
}

void *tgNull_ctor(void* self) {
  return self;
}

void  tgNull_dtor(void* self) {
  (void)self;
}
