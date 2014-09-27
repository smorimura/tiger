/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tlex.h>
#include <tiger/tparse.h>
#include <stdio.h>
#include <error.h>

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

  // Parse and process output.
  tgEnv* env = tgEnv_create(5, NULL);
  tgParser_parse(env, parser, lexer, infile);

  tgFree(lexer);
  tgFree(parser);
  return 0;
}
