/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#ifndef   TG_TPARSE_H
#define   TG_TPARSE_H

/*******************************************************************************
 * The Parser Declaration
 ******************************************************************************/
struct tgParser_ {
};
typedef struct tgParser_ tgParser;

/*******************************************************************************
 * Parser Public Methods
 ******************************************************************************/

tgParser* tgParser_alloc();
void tgParser_free(tgParser* parser);

#endif // TG_TPARSE_H
