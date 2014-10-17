/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tlex.h>
#include <malloc.h>

/*******************************************************************************
 * tgLexer Public Methods
 ******************************************************************************/
tgLexer* tgLexer_alloc() {
  tgLexer* lex = malloc(sizeof(tgLexer));
  return lex;
}

void tgLexer_free(tgLexer* lex) {
  free(lex);
}
