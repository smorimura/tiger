/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tcode.h>
#include <tiger/talloc.h>
#include "tlex.h"
#include <malloc.h>
#include <string.h> /* strcmp */
#include <assert.h>

/* Static */
static void tgKTab_grow(tgKTab* t) {
  size_t curr = t->end - t->begin;
  size_t next = curr * 2 + 1;
  t->begin = realloc(t->begin, sizeof(tgKTab) * next);
  t->end = t->begin + next;
  t->curr = t->begin + curr;
}

static void tgKTab_push(tgKTab* t, tgKData* d) {
  if (t->curr == t->end)
    tgKTab_grow(t);
  *t->curr = *d;
  ++t->curr;
}

static void tgSTab_grow(tgSTab* t) {
  size_t curr = t->end - t->begin;
  size_t next = curr * 2 + 1;
  t->begin = realloc(t->begin, sizeof(tgSTab) * next);
  t->end = t->begin + next;
  t->curr = t->begin + curr;
}

static void tgSTab_push(tgSTab* t, tgSData* d) {
  if (t->curr == t->end)
    tgSTab_grow(t);
  *(t->curr++) = *d;
}

static size_t tgKTab_getInt(tgKTab* t, tgInteger i) {
  tgKTabValue **bucket = &t->map[0];
  tgKTabValue *curr = *bucket;

  /* See if it already exists */
  while (curr) {
    tgKData *data = &t->begin[curr->offset];
    if (data->type == TGK_INTEGER && data->data.integer == i) {
      return curr->offset;
    }
    curr= curr->next;
  }

  /* Create a value */
  {
    tgKData *data;
    tgKTabValue *value;
    data = malloc(sizeof(tgKData));
    data->type = TGK_INTEGER;
    data->data.integer = i;
    tgKTab_push(t, data);

    /* Create an entry */
    value = malloc(sizeof(tgKTabValue));
    value->offset = ((t->curr - 1) - t->begin);
    value->next = *bucket;
    *bucket = value;

    return value->offset;
  }
}

static size_t tgKTab_getReal(tgKTab* t, tgReal r) {
  tgKTabValue **bucket = &t->map[0];
  tgKTabValue *curr = *bucket;

  /* See if it already exists */
  while (curr) {
    tgKData *data = &t->begin[curr->offset];
    if (data->type == TGK_REAL && data->data.real == r) {
      return curr->offset;
    }
    curr= curr->next;
  }

  /* Create a value */
  {
    tgKData *data;
    tgKTabValue *value;

    data = malloc(sizeof(tgKData));
    data->type = TGK_REAL;
    data->data.real = r;
    tgKTab_push(t, data);

    /* Create an entry */
    value = malloc(sizeof(tgKTabValue));
    value->offset = ((t->curr - 1) - t->begin);
    value->next = *bucket;
    *bucket = value;

    return value->offset;
  }
}

static size_t hashstr(tgString s) {
  (void)s;
  return 0;
}

static size_t tgKTab_getStr(tgKTab* t, tgString s) {
  tgKTabValue **bucket = &t->map[hashstr(s) % t->size];
  tgKTabValue *curr = *bucket;

  /* See if it already exists */
  while (curr) {
    tgKData *data = &t->begin[curr->offset];
    if (data->type == TGK_STRING && strcmp(data->data.string, s) == 0) {
      return curr->offset;
    }
    curr= curr->next;
  }

  /* Create a value */
  {
    tgKData *data;
    tgKTabValue *value;

    data = malloc(sizeof(tgKData));
    data->type = TGK_STRING;
    data->data.string = s;
    tgKTab_push(t, data);

    /* Create an entry */
    value = malloc(sizeof(tgKTabValue));
    value->offset = ((t->curr - 1) - t->begin);
    value->next = *bucket;
    *bucket = value;

    return value->offset;
  }
}

static size_t tgSTab_get(tgSTab* t, size_t off) {
  tgSTabValue **bucket = &t->map[off % t->size];
  tgSTabValue *curr = *bucket;

  /* See if it already exists */
  while (curr) {
    tgSData *data = &t->begin[curr->offset];
    if (data->offset == off) {
      return curr->offset;
    }
    curr= curr->next;
  }

  /* Create a value */
  {
    tgSData *data;
    tgSTabValue *value;

    data = malloc(sizeof(tgSData));
    data->offset = off;
    tgSTab_push(t, data);

    /* Create an entry */
    value = malloc(sizeof(tgSTabValue));
    value->offset = (t->curr - t->begin) - 1;
    value->next = *bucket;
    *bucket = value;

    return value->offset;
  }
}

/* Allocate tgCode */
static void tgKTab_init(tgKTab* t) {
  int i = 0;
  t->begin = malloc(sizeof(tgKData) * 5);
  t->curr = t->begin;
  t->end = t->begin + 5;
  t->map = malloc(sizeof(tgKData*)*5);
  for (;i<5;++i) t->map[i] = NULL;
  t->size = 5;
}

static void tgSTab_init(tgSTab* t) {
  int i = 0;
  t->begin = malloc(sizeof(tgSData));
  t->curr = t->begin;
  t->end = t->begin + 1;
  t->map = malloc(sizeof(tgSData*)*5);
  for (;i<5;++i) t->map[i] = NULL;
  t->size = 5;
}

struct tgCode_* tgCode_alloc(struct tgState_* T) {
  struct tgCode_*c = tgAlloc(sizeof(tgCode));
  tgKTab_init(&c->k);
  tgSTab_init(&c->s);
  c->begin = malloc(sizeof(tgRaw));
  c->curr = c->begin;
  c->end = c->begin + 1;
  return c;
}

void tgCode_free(struct tgState_* T, struct tgCode_* c) {
  free(c->begin);
  tgFree(c);
}

/* Write constants */
size_t tgCode_defInt(struct tgCode_* c, tgInteger kint) {
  size_t off = tgKTab_getInt(&c->k, kint);
  return off;
}

size_t tgCode_defReal(tgCode* c, tgReal kreal) {
  size_t off = tgKTab_getReal(&c->k, kreal);
  return off;
}

size_t tgCode_defStr(tgCode* c, tgString kstr) {
  size_t off = tgKTab_getStr(&c->k, kstr);
  return off;
}

size_t tgCode_defSym(tgCode* c, tgString s) {
  size_t soff = tgCode_defStr(c, s);
  tgSTab_get(&c->s, soff);
  return soff;
}

tgInteger tgCode_getInt(tgCode* c, size_t idx) {
  return c->k.begin[idx].data.integer;
}

tgReal tgCode_getReal(tgCode* c, size_t idx) {
  return c->k.begin[idx].data.real;
}

tgString tgCode_getStr(tgCode* c, size_t idx) {
  return c->k.begin[idx].data.string;
}

static void tgCode_grow(tgCode* t) {
  size_t curr = t->end - t->begin;
  size_t next = curr * 2 + 1;
  t->begin = realloc(t->begin, next);
  t->end = t->begin + next;
  t->curr = t->begin + curr;
}

static void tgCode_push(tgCode *c, tgRaw b) {
  if (c->curr == c->end) tgCode_grow(c);
  *(c->curr++) = b;
}

void tgCode_write(tgCode* c, tgByteCode OP) {
  tgCode_push(c, OP);
}

void tgCode_writeA(tgCode* c, tgByteCode OP, size_t A) {
  tgCode_write(c, OP);
  tgCode_push(c, A);
}

void tgCode_writeAB(tgCode* c, tgByteCode OP, size_t A, size_t B) {
  tgCode_writeA(c, OP, A);
  tgCode_push(c, B);
}

void tgCode_writeABC(tgCode* c, tgByteCode OP, size_t A, size_t B, size_t C) {
  tgCode_writeAB(c, OP, A, B);
  tgCode_push(c, C);
}

/* Output to file */
#define BC(b) case b: fprintf(out, "%s", #b); break;
static tgByteCode tgCode_fprintOp(FILE* out, tgByteCode b) {
  switch (b) {
    BC(TB_PUSH)
    BC(TB_PUSHR)
    BC(TB_PUSHK)
    BC(TB_DIV)
    BC(TB_MUL)
    BC(TB_ADD)
    BC(TB_SUB)
    BC(TB_ASSIGN)
    BC(TB_EQ)
    BC(TB_GT)
    BC(TB_GE)
    BC(TB_LT)
    BC(TB_LE)
    BC(TB_JIT)
    BC(TB_RJMP)
    BC(TB_JIF)
    BC(TB_JMP)
    BC(TB_CALL)
    default:
      fprintf(out, "<< ERROR! >>\n");
  }
  return b;
}
#undef BC

static void tgCode_fprintK(FILE* out, tgCode* c, size_t off) {
  tgKData *data = &c->k.begin[off];
  switch (data->type) {
    case TGK_INTEGER:
      fprintf(out, "%d", data->data.integer);
      break;
    case TGK_REAL:
      fprintf(out, "%f", data->data.real);
      break;
    case TGK_STRING:
      fprintf(out, "\"%s\"", data->data.string);
      break;
    default:
      break;
  }
}

void tgCode_fprint(FILE* out, tgCode* c) {
  /* Print Constant Table */
  fprintf(out, "; Begin Constants\n");
  {
    tgKData *curr = c->k.begin;
    while (curr != c->k.curr) {
      fprintf(out, "K(%d) : ", (int)(curr - c->k.begin));
      switch(curr->type) {
        case TGK_INTEGER:
          fprintf(out, "Integer = %d;\n", curr->data.integer);
          break;
        case TGK_STRING:
          fprintf(out, "String  = \"%s\";\n", curr->data.string);
          break;
        case TGK_REAL:
          fprintf(out, "Real    = %f;\n", curr->data.real);
          break;
        default:
          break;
      }
      ++curr;
    }
  }
  fprintf(out, "; End Constants\n\n");

  /* Print Symbol Table */
  /*
  fprintf(out, "; Begin Symbols\n");
  {
    tgSData *curr = c->s.begin;
    while (curr != c->s.curr) {
      tgKData *str = &c->k.begin[curr->offset];
      fprintf(out, "S(%d) := %s\n", (int)(curr - c->s.begin), str->data.string);
      ++curr;
    }
  }
  fprintf(out, "; End Symbols\n\n");
  */

  /* Print Bytecode */
  fprintf(out, "; Begin Bytecode\n");
  {
    tgRaw * curr = c->begin;
    while (curr != c->curr) {
      switch(tgCode_fprintOp(out, *curr)) {
        case TB_PUSH:
        case TB_PUSHR:
          fprintf(out, "   ");
          tgCode_fprintK(out, c, *++curr);
          break;
        case TB_PUSHK:
          fprintf(out, "  ");
          tgCode_fprintK(out, c, *++curr);
          break;
        case TB_CALL:
          fprintf(out, "   ");
          tgCode_fprintK(out, c, *++curr);
          break;
        case TB_JIT:
        case TB_JIF:
        case TB_JMP:
        case TB_RJMP:
          fprintf(out, "    ");
          fprintf(out, "%d", *++curr);
          break;
        default:
          break;
      }
      fprintf(out, "\n");
      ++curr;
    }
  }
  fprintf(out, "; End Bytecode\n");
}

static void tgCode_fprintbK(FILE* out, tgKData *curr) {
  fputc(curr->type, out);
  switch (curr->type) {
    case TGK_INTEGER:
      fwrite(&curr->data.integer, sizeof(tgInteger), 1, out);
      break;
    case TGK_REAL:
      fwrite(&curr->data.real, sizeof(tgReal), 1, out);
      break;
    case TGK_STRING:
    {
      fputs(curr->data.string, out);
      fputc(0, out);
    }
      break;
    default:
      break;
  }
}

void tgCode_fprintb(FILE* out, tgCode* c) {
  /* Print constants */
  {
    tgKData *curr = c->k.begin;
    while (curr != c->k.curr) {
      tgCode_fprintbK(out, curr);
      ++curr;
    }
  }

  /* Print Bytecode */
  {
    tgRaw *curr = c->begin;
    fputc(0xFF, out);
    while (curr != c->curr) {
      fputc(*curr, out);
      ++curr;
    }
  }
}
