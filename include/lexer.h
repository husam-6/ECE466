#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

// Function to parse a directive indicating a new file 
void file_start(char* yytext);

// Function to handle hex fractions (with p-2 at the end for example)
long double hex_frac(char *yytext);

// Convert inputted string for storage
char to_bytes(char * inp, int * jump);

// Convert back to string
char * to_char(unsigned char inp);

// Global for line number
extern int line_num;

// Global to store file name
char * file_name;

// Number type struct
enum num_type{
        U = 0,
        UL,
        ULL, 
        L,
        LL,
        I,
        SH,
        S,
        V,
        C,
        UC,
        D,
        F,
        LD
};

// Number struct
struct number {
    union {
        unsigned long long integer; 
        long double frac; 
    };

    enum num_type type; 
};

// String struct
struct string_literal {
    char *content; 
    int length; 
};

// // yylval
// typedef union {
//           struct string_literal str;
//           char charlit;
//           char *ident;
//           struct number num; 
// } YYSTYPE;

// extern YYSTYPE yylval;





#endif /* LEXER_H */