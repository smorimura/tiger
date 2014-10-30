/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TIGER_TBINARY_H
#define   TIGER_TBINARY_H

#include <tiger/fwdtiger.h> /* tgState */

struct tgBinary_ {
  tgRaw *begin;
  tgRaw *curr;
  tgRaw *end;
};
typedef struct tgBinary_ tgBinary;

tgBinary* tgBinary_alloc(tgState* T);
void tgBinary_free(tgState* T, tgBinary* b);

void tgBinary_read(tgState* T, tgBinary* b, tgCode* c, tgRaw* start, tgRaw* end);

#endif // TIGER_TBINARY_H
