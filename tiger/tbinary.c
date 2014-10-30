/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tbinary.h>
#include <tiger/talloc.h>
#include <tiger/tbcode.h>

typedef int (*tgCallback)(tgState*);

int tgBinary_add(tgState* T) {
  // Addition
}

void tgBinary_write(tgState* T, )

tgBinary* tgBinary_alloc(tgState* T) {
  tgBinary *b = tgAlloc(sizeof(tgBinary));
  b->begin = tgAlloc(sizeof(tgRaw));
  b->curr = b->begin;
  b->end = b->begin;
}

void tgBinary_free(tgState* T, tgBinary* b) {
  tgFree(b->begin);
  tgFree(b);
}

void tgBinary_read(tgState* T, tgBinary* b, tgCode* c, tgRaw* start, tgRaw* end) {
  while (start != end) {
    switch (*start) {
      case TB_ADD:
        tgBinary_write(b, &tgBinary_add);
        break;
      case TB_SUB:
        tgBinary_write(b, &tgBinary_sub);
        break;
      case TB_MUL:
        tgBinary_write(b, &tgBinary_mul);
        break;
      case TB_DIV:
        tgBinary_write(b, &tgBinary_div);
        break;
      case TB_ASSIGN:
        tgBinary_write(b, &tgBinary_assign);
        break;
      case TB_CALL:
        tgBinary_writeCall(b, c, *++start);
        break;
      case TB_EQ:
        tgBinary_write(b, &tgBinary_testeq);
        break;
      case TB_JIF:
        tgBinary_write(b, &tgBinary_jif);
        tgBinary_writeA(b, *++start);
        break;
      case TB_JMP:
        tgBinary_write(b, &tgBinary_jmp);
        tgBinary_writeA(b, *++start);
        break;
      case TB_PUSH:
        tgBinary_write(b, &tgBinary_push);
        tgBinary_writeSym(b, c, *++start);
        break;
      case TB_PUSHK:
        tgBinary_write(b, &tgBinary_push);
        tgBinary_writeK(b, c, *++start);
        break;
    }
    ++start;
  }
}

void tgBinary_run(tgState* T, tgBinary* b) {
  tgRaw *curr = b->begin;
  while (curr != b->end) {
    int rval = ((tgCallback)curr)(T);
    curr += sizeof(tgCallback);
  }
}
