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
typedef int (*tgRead)(tgBuffer*);
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
// Reader Functions
int tgRead_readFile(tgBuffer* b);

// Allocation Routines
tgBuffer* tgBuffer_allocFile(tgState* T, const char* filename);
tgBuffer* tgBuffer_allocLiteral(tgState* T, const char* str);
void tgBuffer_freeFile(tgState* T, tgBuffer* b);
void tgBuffer_freeLiteral(tgState* T, tgBuffer* b);

// Generic Functionality
char tgBuffer_next(tgBuffer* b);

#endif //   TIGER_TBUFFER_H
