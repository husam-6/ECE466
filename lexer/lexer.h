#include<stdio.h>
#include<stdlib.h>
#include<math.h>

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

// yylval
typedef union {
          char *string_literal;
          char *charlit;
          char *ident;
          struct number num; 
} YYSTYPE;

extern YYSTYPE yylval;

// Globals to store file name and line number
char * file_name;
int line_num = 1;

void file_start(char* yytext){
    // Extract line number and file name
    char* str_mod = strdup(yytext);
    char* token = strtok(str_mod, " ");
    
    // Split file and loop through each element
    int i = 0; 
    while( token != NULL ) {
        // Save line number
        if (i == 1){
            line_num = atoi(token);
        }
        // Save file name
        else if (i == 2){
            file_name = token+1;
            file_name[strlen(file_name)-1] = '\0'; 
        }
        token = strtok(NULL, " ");
        i++; 
    }
}

// function to convert octalNumber to decimal
long long octal_to_dec(int octalNumber) {
    int decimalNumber = 0, i = 0;

    while(octalNumber != 0) {
        decimalNumber += (octalNumber%10) * pow(8,i);
        ++i;
        octalNumber/=10;
    }

    i = 1;

    return decimalNumber;
}

// Function to handle hex fractions (with p-2 at the end for example)
long double hex_frac(char *yytext){
    char *hex_p; 
    long double hex_base  = strtoull(yytext, &hex_p, 16); 
    int tmp = atoi(hex_p + 1);
    return hex_base * pow(2, tmp);
}