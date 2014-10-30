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
  fclose(stdin);
  return 0;
}

int main(int argc, char const* argv[]) {

  // Create and initialize TigerScript
  tgState *T = tgState_create();

  // Add helper functions for the end-user
  tgState_addCFunc(T, "help", &help);
  tgState_addCFunc(T, "quit", &quit);

  while (!feof(file)) {
    printf("> ");
    tgState_exec(T, io);
  }

  // Clean up TigerScript
  tgState_free(T);

  return 0;
}
