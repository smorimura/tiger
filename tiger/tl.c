/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tlex.h>
#include <tiger/tparse.h>
#include <stdio.h>

int main(int argc, char const* argv[]) {
  tgLexer *lexer = tgAlloc(tgLexer);
  tgParser*parser= tgAlloc(tgParser);

  FILE* infile = stdin;
  if (argc > 1) {
    infile = fopen(argv[1], "rw");
  }

  // Parse and process output.
  tgParser_parse(parser, lexer, infile);

  tgFree(lexer);
  tgFree(parser);
  return 0;
}
