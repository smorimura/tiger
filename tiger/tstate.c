/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tstate.h>
#include <tiger/tenv.h>
#include <tiger/tlex.h>
#include <tiger/tparse.h>
#include <tiger/tbuffer.h>
#include <malloc.h>

/*******************************************************************************
 * tgState Private Methods
 ******************************************************************************/

static void tgState_execBuffer(tgState* T, tgBuffer* buffer) {
  tgLexer parser;
  tgLexer_init(T, &parser, buffer);
  do {
    tgParser_parse(&parser);
  } while (parser.tokens[0].tag != TG_EOF);
  tgLexer_dinit(&parser);
}

static tgCode* tgState_compileBuffer(tgState* T, tgBuffer* buffer) {
  tgLexer parser;
  tgCode *finalCode;

  tgLexer_init(T, &parser, buffer);
  do {
    tgParser_parse(&parser);
  } while (parser.tokens[0].tag != TG_EOF);
  finalCode = parser.code;
  tgLexer_dinit(&parser);

  return finalCode;
}

/*******************************************************************************
 * tgState Public Methods
 ******************************************************************************/

/**
 * @brief Creates a new tgState with no symbols, constants, or bytecode.
 *
 * @param alloc The allocator to use within this state.
 * @param ud The userdata that will be passed to \p alloc.
 * @return tgState*
 */
tgState* tgState_create(tgAllocator alloc, void* ud) {
  tgState* T = alloc(ud, NULL, sizeof(tgState));
  T->allocator = alloc;
  T->allocatorData = ud;
  T->env = tgEnv_alloc(T, 5, NULL);
  T->stack = alloc(ud, NULL, sizeof(tgValue*) * 100);
  T->top = T->stack;
  T->ktab = alloc(ud, NULL, sizeof(tgValue) * 100 );
  T->ktop = T->ktab;
  return T;
}

/**
 * @brief Frees a state and all symbols, constants, and bytecode associated.
 *
 * @param T The state that should be freed.
 * @return void
 */
void tgState_free(tgState* T) {
  tgEnv_free(T->env);
  free(T);
}

int tgState_addCFunc(tgState* T, char const* fname, tgCallback func) {
  tgSymbol* sym = tgEnv_newSym(T, T->env, fname);

  if(!sym) return 0;

  sym->data = func;
  sym->tag = TG_CFUNCTION;

  return 1;
}

void tgState_execFile(tgState* T, char const* path) {
  tgCode *c = tgState_compileFile(T, path);
  tgState_execCode(T, c);
}

void tgState_execLiteral(tgState* T, char const* str) {
  tgCode *c = tgState_compileLiteral(T, str);
  tgState_execCode(T, c);
}

void tgState_execCode(tgState* T, tgCode* c) {
  tgRaw *curr = c->begin;
  while (curr < c->curr) {
    switch (*(curr++)) {
      case TB_PUSHR:
      {
        tgString k = tgCode_getStr(c, *(curr++));
        tgSymbol* sym = tgEnv_getSym(T->env, k);
        if (!sym)
          sym = tgEnv_newSym(T, T->env, k);
        tgState_pushPtr(T, &sym->data);
      }
      break;
      case TB_PUSH:
      {
        tgString k = tgCode_getStr(c, *(curr++));
        tgSymbol* sym = tgEnv_getSym(T->env, k);
        if (!sym)
          sym = tgEnv_newSym(T, T->env, k);
        tgState_push(T, sym->data);
      }
        break;
      case TB_PUSHK:
      {
        tgKData k = c->k.begin[*(curr++)];
        switch (k.type) {
          case TGK_INTEGER:
            tgState_pushInt(T, k.data.integer);
            break;
          case TGK_STRING:
            tgState_pushStr(T, k.data.string);
            break;
        }
      }
        break;
      case TB_ASSIGN:
        tgState_callb(T, TB_ASSIGN);
        break;
      case TB_ADD:
        tgState_callb(T, TB_ADD);
        break;
      case TB_SUB:
        tgState_callb(T, TB_SUB);
        break;
      case TB_DIV:
        tgState_callb(T, TB_DIV);
        break;
      case TB_MUL:
        tgState_callb(T, TB_MUL);
        break;
      case TB_CALL:
      {
        tgString k = tgCode_getStr(c, *(curr++));
        tgSymbol* sym = tgEnv_getSym(T->env, k);
        tgCallback cb = (tgCallback)sym->data;
        tgState_call(T, cb);
      }
      break;
      case TB_EQ:
        tgState_callb(T, TB_EQ);
        break;
      case TB_LT:
        tgState_callb(T, TB_LT);
        break;
      case TB_GT:
        tgState_callb(T, TB_GT);
        break;
      case TB_LE:
        tgState_callb(T, TB_LE);
        break;
      case TB_GE:
        tgState_callb(T, TB_GE);
        break;
      case TB_RJMP:
      {
        tgRaw r = *(curr++);
        curr -= r;
      }
      break;
      case TB_JMP:
      {
        tgRaw r = *(curr++);
        curr += r;
      }
      break;
      case TB_JIT:
      {
        tgValue* v = tgState_pop(T);
        tgRaw r = *(curr++);
        if (v->data.integer)
          curr += r;
      }
      break;
      case TB_JIF:
      {
        tgValue* v = tgState_pop(T);
        tgRaw r = *(curr++);
        if (!v->data.integer)
          curr += r;
      }
      break;
    }
  }
}

tgCode* tgState_compileFile(tgState* T, char const* path) {
  tgCode *c = NULL;
  tgBuffer *b = tgBuffer_allocFile(T, path);
  if (b) {
    c = tgState_compileBuffer(T, b);
    tgBuffer_freeFile(T, b);
  }
  return c;
}

tgCode* tgState_compileLiteral(tgState* T, const char* str) {
  tgCode *c = NULL;
  tgBuffer *b = tgBuffer_allocLiteral(T, str);
  if (b) {
    c = tgState_compileBuffer(T, b);
    tgBuffer_freeLiteral(T, b);
  }
  return c;
}

static void tgPrintAttrib(tgToken* t) {
  switch (t->type) {
    case TA_INTEGER:
      printf("<%d>", t->attrib.integer);
      break;
    case TA_REAL:
      printf("<%f>", t->attrib.real);
      break;
    case TA_STRING:
      printf("<%s>", t->attrib.string);
      break;
  }
}

void tgState_pushk(tgState* T, tgValue* v) {
  *T->top = *v;
  ++T->top;
}

void tgState_push(tgState* T, tgValue* v) {
  T->top->data.pointer = v;
  ++T->top;
}

void tgState_pushInt(tgState* T, tgInteger k) {
  T->top->data.integer = k;
  ++T->top;
}

void tgState_pushStr(tgState* T, tgString k) {
  T->top->data.string = k;
  ++T->top;
}

void tgState_pushPtr(tgState* T, void* k) {
  T->top->data.pointer = k;
  ++T->top;
}

tgValue* tgState_pop(tgState* T) {
  return --T->top;
}

void tgState_call(tgState* T, tgCallback cb) {
  cb(T);
}

tgValue* tgState_defK(tgState* T, tgValue v) {
  *T->ktop = v;
  return T->ktop++;
}

tgValue* tgState_defS(tgState* T, tgValue* kn) {
  tgSymbol* sym = tgEnv_newSym(T, T->env, kn->data.string);
  sym->data = tgAlloc(sizeof(tgValue));
  return (tgValue*)sym->data;
}

#define arop(op) {\
  tgValue* v2 = tgState_pop(T);\
  tgValue* v1 = tgState_pop(T);\
  tgState_pushInt(T, v1->data.integer op v2->data.integer);}

#define reop(op) {\
  tgValue* v2 = tgState_pop(T);\
  tgValue* v1 = tgState_pop(T);\
  tgState_pushInt(T, (v1->data.integer op v2->data.integer) ? 1 : 0);}

void tgState_callb(tgState* T, tgByteCode c) {
  switch (c) {
    case TB_ADD:
      arop(+);
      break;
    case TB_SUB:
      arop(-);
      break;
    case TB_MUL:
      arop(*);
      break;
    case TB_DIV:
      arop(/);
      break;
    case TB_ASSIGN:
    {
      tgValue* v2 = tgState_pop(T);
      tgValue* v1 = tgState_pop(T);
      *(tgValue*)(v1->data.pointer) = *v2;
    }
      break;
    case TB_LT:
      reop(<);
      break;
    case TB_GT:
      reop(>);
      break;
    case TB_LE:
      reop(<=);
      break;
    case TB_GE:
      reop(>=);
      break;
    case TB_EQ:
      reop(==);
      break;
  }
}
