/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TIGER_TOPCODE_H
#define   TIGER_TOPCODE_H

enum tgByteCode_ {
/*******************************************************************************
 * ByteCode is the state of the code before pointers are patched into the
 * runtime operation. Pointer patching will translate ByteCode to highly
 * efficient opcode that will not require extra checking on the VM's side.
 * Definitions:
 * R(N) = The Nth register.
 * S(N) = Access the Nth symbol.
 * K(N) = Access the Nth constant.
 ******************************************************************************/
/*******************************************************************************
 * name          | args  | Technical Description   | Side Effect               *
 ******************************************************************************/
  // Memory Manipulation
  TB_PUSH,     /*| A     | OP_MOVE Rtop S(A)       | N/A*/
  TB_PUSHK,    /*| A     | OP_MOVE Rtop K(A)       | N/A*/
  TB_PUSHR,    /*| A     | OP_MOVE Rtop &S(A)      | N/A*/

  // Arithmetic Operations
  TB_DIV,      /*|       | OP_ADD  Rtop Rtop Rtop-1| N/A*/
  TB_ADD,TB_MUL,TB_SUB,TB_ASSIGN,

  // Relational Operations
  TB_EQ,TB_LT,TB_LE,TB_GT,TB_GE,

  // Jumps
  TB_JMP,TB_JIT,TB_JIF,TB_RJMP,

  // Calls
  TB_CALL,

  // Reserved for extended operations
  TB_RESERVED = 0xFF
};
typedef enum tgByteCode_ tgByteCode;

#endif // TIGER_TOPCODE_H
