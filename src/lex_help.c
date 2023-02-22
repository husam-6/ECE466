#include "lexer.h"
#include <string.h>

// File start information
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

// Function to handle hex fractions (with p-2 at the end for example)
long double hex_frac(char *yytext){
    char *hex_p; 
    long double hex_base  = strtoull(yytext, &hex_p, 16); 
    int tmp = atoi(hex_p + 1);
    return hex_base * pow(2, tmp);
}

void slice(const char *str, char *result, size_t start, size_t end){
        strncpy(result, str + start, end - start);
        result[end-start + 1] = '\0';
    }

// Convert inputted string for storage
char to_bytes(char * inp, int * jump){ 
    // If escape sequence...
    if (inp[0] == '\\'){
        inp += 1;
        // Hex sequences
        if ((inp[0] == 'x' || inp[0] == 'X')){
                inp += 1;
                int tmp;
                sscanf(inp, "%2x%n", &tmp, jump);
                (*jump)+=2;
                return (char) tmp;
            }
        // Octal first digit
        else if (inp[0] <= '7' && inp[0] >= '0'){
                // printf("IM HERE %s\n", inp);
                int tmp;
                sscanf(inp, "%3o%n", &tmp, jump);
                (*jump)++;
                // printf("%d\n", tmp);
                return (char) tmp;
            }
        // Otherwise store escape character code
        else  {
            *jump = 2; 
            switch (inp[0]){
                case 'a':    {return '\a';}
                case 'b':    {return '\b';}
                case 'f':    {return '\f';}
                case 'n':    {return '\n';}
                case 'r':    {return '\r';}
                case 't':    {return '\t';}
                case 'v':    {return '\v';}
                case '0':    {return '\0';}
                case '\'':   {return '\'';}
                case '\\':   {return '\\';}
                case '\"':   {return '\"';}
                case '\?':   {return '\?';}
                default:     {fprintf(stderr, "BAD: INVALID ESCAPED CHAR"); exit(2);}
            }
        }
    }

    // Regular char
    (*jump) = 1; 
    return inp[0];
}

char * to_char(unsigned char inp){
    char * o;
    o = "bruh";
    switch (inp){
        case '\a':    {return "\\a";}
        case '\b':    {return "\\b";}
        case '\f':    {return "\\f";}
        case '\n':    {return "\\n";}
        case '\r':    {return "\\r";}
        case '\t':    {return "\\t";}
        case '\v':    {return "\\v";}
        case '\0':    {return "\\0";}
        case '\'':    {return "\\'";}
        case '\\':    {return "\\\\";}
        case '\"':    {return "\\\"";}
        case '\?':    {return "\?";}
    }
    if (inp >= 33 && inp <= 126){
        asprintf(&o, "%c", inp);
        return o; 
    }
    asprintf(&o, "\\%03o", inp);
    return o; 
}