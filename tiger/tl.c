/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tiger.h>
#include <stdio.h>
#include <err.h>

static int exit = 0;

int help(tgState* T) {
  printf("Under Construction\n");
  return 0;
}

int quit(tgState* T) {
  exit = 1;
  return 0;
}

int main(int argc, char const* argv[]) {

  // Create and initialize TigerScript
  tgState *T = tgState_alloc();

  // Add helper functions for the end-user
  tgState_addCFunc(T, "help", &help);
  tgState_addCFunc(T, "quit", &quit);

  // Select source to parse input from
  FILE* infile = stdin;
  if (argc > 1) {
    infile = fopen(argv[1], "rw");
    if (infile == NULL) {
      err(1, "Could not open the script provided.");
    }
  }

  // Process input
  char inbuff[4096];
  while (!feof(infile)) {
    printf("< ");
    fgets(inbuff, sizeof(inbuff), infile);
    tgState_execStr(T, inbuff);
  }

  // Clean up TigerScript
  tgState_free(T);

  return 0;
}
