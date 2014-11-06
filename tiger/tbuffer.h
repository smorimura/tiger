/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TIGER_TBUFFER_H
#define   TIGER_TBUFFER_H

#include <stddef.h> /* size_t */
#include <tiger/fwdtiger.h> /* tgState, tgBuffer */

/*******************************************************************************
 * Buffer Declarations
 ******************************************************************************/
typedef int (*tgRead)(struct tgBuffer_*);
struct tgBuffer_{
  char       *begin;
  char const *curr;
  char const *end;
  tgRead read;
  void *data;
};

/*******************************************************************************
 * Public Functions
 ******************************************************************************/
/* Reader Functions */
int tgRead_readFile(struct tgBuffer_* b);

/* Allocation Routines */
struct tgBuffer_* tgBuffer_allocFile(struct tgState_* T, const char* filename);
struct tgBuffer_* tgBuffer_allocLiteral(struct tgState_* T, const char* str);
void tgBuffer_freeFile(struct tgState_* T, struct tgBuffer_* b);
void tgBuffer_freeLiteral(struct tgState_* T, struct tgBuffer_* b);

/* Generic Functionality */
char tgBuffer_next(struct tgBuffer_* b);

#endif /* TIGER_TBUFFER_H */
