#ifndef _UTIL_H_
#define _UTIL_H_

#define MAXTOKENLEN 64

/* assume that strlen(errorMessage) <= 63 */
char errorMessage[64];
TokenType getToken();

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */
void printToken(TokenType, const char*);

#endif




