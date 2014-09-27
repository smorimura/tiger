/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tlex.h>
#include <tiger/tparse.h>
#include <stdio.h>
#include <error.h>
#include <stdlib.h>

void tgExit() {
  exit(0);
}

void tgHelloC() {
  printf("Hello, C.\n");
}

int main(int argc, char const* argv[]) {
  tgLexer *lexer = tgAlloc(tgLexer);
  tgParser*parser= tgAlloc(tgParser);

  FILE* infile = stdin;
  if (argc > 1) {
    infile = fopen(argv[1], "rw");
    if (infile == NULL) {
      error(1, 1, "Could not open the script provided.");
    }
  }

  // Generate the starting environment
  tgEnv* env = tgEnv_create(5, NULL);

  // Create functions within Tiger.
  tgToken* function = tgTId_create("exit");
  function->tag = TG_FUNCTION;
  tgEnv_add(env, (tgTId*)function, &tgExit);
  function =  tgTId_create("callback");
  function->tag = TG_FUNCTION;
  tgEnv_add(env, (tgTId*)function, &tgHelloC);

  // Run scripting environment
  tgParser_parse(env, parser, lexer, infile);

  tgFree(lexer);
  tgFree(parser);
  return 0;
}
