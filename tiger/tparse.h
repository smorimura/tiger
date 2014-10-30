/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TG_TPARSE_H
#define   TG_TPARSE_H

#include <tiger/fwdtiger.h> /* tgState, tgLexer, tgCode */

// The lexer is the parser
typedef tgLexer tgParser;

/*******************************************************************************
 * Parser Public Methods
 ******************************************************************************/

tgParser* tgParser_alloc(tgState* T, tgLexer* lex);
void tgParser_free(tgState* T, tgParser* parser);

void tgParser_parse(tgParser* parser);

#endif // TG_TPARSE_H
