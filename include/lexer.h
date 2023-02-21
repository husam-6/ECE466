#pragma once 
#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

// Number type struct
enum num_type{
        U = 0,
        UL,
        ULL, 
        L,
        LL,
        D,
        F,
        LD,
        I
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

// Global to store file name
char * file_name;

#endif /* LEXER_H */