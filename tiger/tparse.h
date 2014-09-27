/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TG_TPARSE_H
#define   TG_TPARSE_H

#include <tiger/talloc.h>
#include <tiger/tlex.h>
#include <tiger/tenv.h>

typedef struct {
} tgParser;

extern tgClass tgParser_class;
extern mpf_t eax;

void tgParser_parse(tgEnv* env, tgParser* parser, tgLexer* lexer, FILE* infile);

#endif // TG_TPARSE_H
