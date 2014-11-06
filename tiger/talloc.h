/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TG_TALLOC_H
#define   TG_TALLOC_H

#include <stddef.h> /* size_t */
#include <tiger/fwdtiger.h> /* tgState */

typedef void* (*tgAllocator)(void*,void*,size_t);

/* Default memory allocator (malloc/realloc/free) */
void* tgMalloc(void* ud, void* ptr, size_t req);

/* Memory allocation routines */
void* tgAlloc_(struct tgState_* T, size_t s);
void* tgAllocChk_(struct tgState_* T, size_t s, char const* file, size_t line);
void  tgFree_(struct tgState_* T, void* ptr);
void  tgFreeChk_(struct tgState_* T, void* ptr, char const* file, size_t line);

#ifdef TIGER_MEMCHECK
# define tgAlloc(s) tgAllocChk_(T, s, __FILE__, __LINE__)
# define tgFree(obj) tgFreeChk_(T, obj, __FILE__, __LINE__)
#else
# define tgAlloc(s) tgAlloc_(T, s)
# define tgFree(obj) tgFree_(T, obj)
#endif

#endif /* TG_TALLOC_H */
