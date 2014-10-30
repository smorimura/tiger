/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/topcode.h>

#define GET_OPCODE(T) (*(T->rix++))
#define GET_VALUE(T)  (base[*(T->rix++)])

#define tgAdd(T,a,b)    ((a)+(b))
#define tgSub(T,a,b)    ((a)-(b))
#define tgMul(T,a,b)    ((a)*(b))
#define tgDiv(T,a,b)    ((a)/(b))

#define arith_op(T,op)\
{\
  a = GET_VALUE(T); b = GET_VALUE(T);\
  op(T,a,b); \
}

void tgVM_exec(tgState* T) {
  tgOpcode op;
  tgValue *base = T->stack;
  for (;;) {
    op = GET_OPCODE(T);
    switch (op) {
      case OP_ADDRR:
        arith_op(T, tgAdd);
        break;
      case OP_SUBRR:
        arith_op(T, tgSub);
        break;
      case OP_MULRR:
        arith_op(T, tgMul);
        break;
      case OP_DIVRR:
        arith_op(T, tgDiv);
        break;
    }
  }
}
