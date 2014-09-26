/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TG_TALLOC_H
#define   TG_TALLOC_H

#include <stddef.h>

typedef struct {
  size_t size;
  void* (*ctor)(void*);
  void  (*dtor)(void*);
} tgClass;

void* tgMalloc(const tgClass* tclass, size_t bytes);
void* tgAlloc_(const tgClass* tclass);
void  tgFree(void* inst);

void *tgNull_ctor(void*);
void  tgNull_dtor(void*);

#define tgAlloc(class) (class*)tgAlloc_((tgClass const*)&class##_class)

#endif // TG_TALLOC_H
