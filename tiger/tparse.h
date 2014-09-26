/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TG_TPARSE_H
#define   TG_TPARSE_H

#include <tiger/talloc.h>
#include <tiger/tlex.h>

typedef struct {
} tgParser;

extern tgClass tgParser_class;

void tgParser_parse(tgParser* parser, tgLexer* lexer, FILE* infile);

#endif // TG_TPARSE_H
