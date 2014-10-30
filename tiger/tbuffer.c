/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tbuffer.h>
#include <tiger/talloc.h>
#include <tiger/tdef.h>
#include <stdio.h>
#include <string.h>

int tgBuffer_readFile(tgBuffer* b) {
  // Copy unread portion to start of buffer
  memcpy(b->begin, b->curr, b->end - b->curr);

  // Read in file contents, and adjust the vector representation
  int bytes = fread(b->begin, sizeof(char), TIGER_FSBLOCK, (FILE*)b->data);
  b->curr = b->begin;
  b->end = b->begin + bytes;

  return bytes;
}

int tgBuffer_readLiteral(tgBuffer* b) {
  ++b->end;
  return (*b->curr == '\0') ? 0 : 1;
}

tgBuffer* tgBuffer_allocFile(tgState* T, const char* path) {
  FILE* file = fopen(path, "rw");
  if (file) {
    tgBuffer *b = tgAlloc(sizeof(tgBuffer));
    b->begin = tgAlloc(sizeof(char) * TIGER_FSBLOCK);
    b->curr = b->begin - 1;
    b->end = b->begin;
    b->read = &tgBuffer_readFile;
    b->data = file;
    return b;
  }
  return NULL;
}

tgBuffer* tgBuffer_allocLiteral(tgState* T, char const* str) {
  tgBuffer *b = tgAlloc(sizeof(tgBuffer));
  b->begin = (char*)str;
  b->curr = b->begin - 1;
  b->end = b->begin;
  b->read = &tgBuffer_readLiteral;
  return b;
}

void tgBuffer_freeFile(tgState* T, tgBuffer* b) {
  fclose((FILE*)b->data);
  tgFree(b->begin);
  tgFree(b);
}

void tgBuffer_freeLiteral(tgState* T, tgBuffer* b) {
  tgFree(b);
}

char tgBuffer_next(tgBuffer* b) {
  // Adjust buffer information
  if (++b->curr == b->end) {
    if (b->read(b) == 0) {
      --b->curr;
      return EOF;
    }
  }

  // Return the current value
  return *b->curr;
}
