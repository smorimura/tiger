/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TIGER_TCODEGEN_H
#define   TIGER_TCODEGEN_H

#include <stddef.h> /* size_t */
#include <stdio.h> /* FILE */
#include <tiger/fwdtiger.h> /* tgState */
#include <tiger/tbcode.h>
#include <tiger/tvalue.h>

/*******************************************************************************
 * tgKTab - Constant Table
 ******************************************************************************/

// Constant Lookup
struct tgKTabValue_ {
  size_t offset;
  struct tgKTabValue_ *next;
};
typedef struct tgKTabValue_ tgKTabValue;

enum tgKDataType_ {
  TGK_INVALID,
  TGK_INTEGER,
  TGK_REAL,
  TGK_STRING
};
typedef enum tgKDataType_ tgKDataType;

// Constant Value
struct tgKData_ {
  union {
    tgInteger integer;
    tgString string;
    tgReal real;
  } data;
  tgKDataType type;
};
typedef struct tgKData_ tgKData;

// Constant table
struct tgKTab_ {
  size_t size;       // Hashmap<size_t>::size
  tgKTabValue **map; // Hashmap<size_t>::data
  tgKData *begin;    // Vector<tgKData>::begin
  tgKData *curr;     // Vector<tgKData>::curr
  tgKData *end;      // Vector<tgKData>::end
};
typedef struct tgKTab_ tgKTab;

/*******************************************************************************
 * tgSTab - Symbol Table
 ******************************************************************************/

struct tgSTabValue_ {
  size_t offset;
  struct tgSTabValue_ *next;
};
typedef struct tgSTabValue_ tgSTabValue;

struct tgSData_ {
  size_t offset;
};
typedef struct tgSData_ tgSData;

struct tgSTab_ {
  size_t size;
  tgSTabValue **map;
  tgSData *begin;
  tgSData *curr;
  tgSData *end;
};
typedef struct tgSTab_ tgSTab;

/*******************************************************************************
 * tgCode - Code Generation
 ******************************************************************************/
typedef unsigned char tgRaw;
struct tgCode_ {
  tgKTab k;
  tgSTab s;
  tgRaw *begin;
  tgRaw *curr;
  tgRaw *end;
};
typedef struct tgCode_ tgCode;

// Allocate tgCode
tgCode* tgCode_alloc(tgState* T);
void tgCode_free(tgState* T, tgCode* c);

// Write constants
size_t tgCode_defInt(tgCode* c, tgInteger kint);
size_t tgCode_defReal(tgCode* c, tgReal kreal);
size_t tgCode_defStr(tgCode* c, tgString kstr);
size_t tgCode_defSym(tgCode* c, tgString s);

// Get constants
tgInteger tgCode_getInt(tgCode* c, size_t idx);
tgReal tgCode_getReal(tgCode* c, size_t idx);
tgString tgCode_getStr(tgCode* c, size_t idx);

// Write bytecode
void tgCode_write(tgCode* c, tgByteCode OP);
void tgCode_writeA(tgCode* c, tgByteCode OP, size_t A);
void tgCode_writeAB(tgCode* c, tgByteCode OP, size_t A, size_t B);
void tgCode_writeABC(tgCode* c, tgByteCode OP, size_t A, size_t B, size_t C);

// Bytecode Operations
#define tgCode_move(c,s)      tgCode_writeA(c,TB_MOVE,s)
#define tgCode_load(c,idx,s)  tgCode_writeA(c,TB_PUSH,s)
#define tgCode_loadk(c,idx,k) tgCode_writeA(c,TB_PUSHK,k)
#define tgCode_jit(c)         tgCode_writeA(c,TB_JIT,0xFF)
#define tgCode_jif(c)         tgCode_writeA(c,TB_JIF,0xFF)
#define tgCode_jmp(c)         tgCode_writeA(c,TB_JMP,0xFF)
#define tgCode_rjmp(c)        tgCode_writeA(c,TB_RJMP,0xFF)

// Output to file
void tgCode_fprint(FILE* out, tgCode* c);
void tgCode_fprintb(FILE* out, tgCode* c);

#endif // TIGER_TCODEGEN_H
