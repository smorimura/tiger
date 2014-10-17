/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tparse.h>
#include <malloc.h>

tgParser* tgParser_alloc() {
  tgParser* parser = malloc(sizeof(tgParser));
  return parser;
}

void tgParser_free(tgParser* parser) {
  free(parser);
}
