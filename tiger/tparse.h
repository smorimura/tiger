/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TG_TPARSE_H
#define   TG_TPARSE_H

#include <tiger/fwdtiger.h> /* tgState, tgLexer, tgCode */

/* The lexer is the parser */
typedef struct tgLexer_ tgParser;

/*******************************************************************************
 * Parser Public Methods
 ******************************************************************************/

tgParser* tgParser_alloc(struct tgState_* T, struct tgLexer_* lex);
void tgParser_free(struct tgState_* T, tgParser* parser);

void tgParser_parse(tgParser* parser);

#endif /* TG_TPARSE_H */
