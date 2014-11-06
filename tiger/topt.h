/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TIGER_TOPT_H
#define   TIGER_TOPT_H

#include <stdio.h> /* FILE */

typedef void (*commandCB)(int *argc, char const* argv[]);

struct tgOpt_ {
  char const  shortopt;
  char const* longopt;
  commandCB   callback;
  char const* description;
};
typedef struct tgOpt_ tgOpt;

int tgOpt_parse(int argc, const char* argv[], tgOpt const opts[]);
int tgOpt_proccess(int argc, char const *argv[], int *idx, tgOpt const opts[]);
void tgOpt_fprint(FILE *f, tgOpt const *opts);

#endif /* TIGER_TOPT_H */
