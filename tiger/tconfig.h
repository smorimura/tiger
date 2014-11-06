/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TIGER_TCONFIG_H
#define   TIGER_TCONFIG_H

#include <string.h>

typedef float tgReal;
#define tgRealFromFloat(real, float) (real = float)

typedef int tgInteger;
#define tgIntegerFromInt(integer, whole) (integer = whole)

typedef char* tgString;
#define tgStringFromCharPtr(string, charPtr) (string = malloc(sizeof(char) * strlen(charPtr) + 1), strcpy(string, charPtr))

#endif /* TIGER_TCONFIG_H */
