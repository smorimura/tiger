/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TIGER_TVALUE_H
#define   TIGER_TVALUE_H

#include <tiger/tconfig.h>

union tgData_ {
  tgInteger integer;
  tgReal real;
  tgString string;
  void* pointer;
};
typedef union tgData_ tgData;

struct tgValue_ {
  tgData data;
};
typedef struct tgValue_ tgValue;

#endif // TIGER_TVALUE_H
