#ifndef _SCAN_H_
#define _SCAN_H_

/* MAXTOKENLEN is the maximum size of a token */
#define MAXTOKENLEN 64

/* lexeme of identifier or reserved word */
extern char tokenString[MAXTOKENLEN + 1];

/* assume that strlen(errorMessage) <= 63 */
extern char errorMessage[64];

/* function getToken returns the
 * next token in source file */
TokenType getToken();

#endif

