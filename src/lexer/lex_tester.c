// Testing file for lexer - only to be used to verify that lexical analysis
// captures information accurately 

#include "lexer.h"
#include "parser.tab.h"



int main() {
    int t;
    while((t=yylex()))
        {
        switch(t) {
                // All cases for keyword match
                case AUTO:       {printf("%s\t%d\t%s\n", file_name, line_num, "AUTO"); break;}
                case BREAK:      {printf("%s\t%d\t%s\n", file_name, line_num, "BREAK"); break;}
                case CASE:       {printf("%s\t%d\t%s\n", file_name, line_num, "CASE"); break;}
                case CHAR:       {printf("%s\t%d\t%s\n", file_name, line_num, "CHAR"); break;}
                case CONST:      {printf("%s\t%d\t%s\n", file_name, line_num, "CONST"); break;}
                case CONTINUE:   {printf("%s\t%d\t%s\n", file_name, line_num, "CONTINUE"); break;}
                case DEFAULT:    {printf("%s\t%d\t%s\n", file_name, line_num, "DEFAULT"); break;}
                case DO:         {printf("%s\t%d\t%s\n", file_name, line_num, "DO"); break;}
                case DOUBLE:     {printf("%s\t%d\t%s\n", file_name, line_num, "DOUBLE"); break;}
                case ELSE:       {printf("%s\t%d\t%s\n", file_name, line_num, "ELSE"); break;}
                case ENUM:       {printf("%s\t%d\t%s\n", file_name, line_num, "ENUM"); break;}
                case EXTERN:     {printf("%s\t%d\t%s\n", file_name, line_num, "EXTERN"); break;}
                case FLOAT:      {printf("%s\t%d\t%s\n", file_name, line_num, "FLOAT"); break;}
                case FOR:        {printf("%s\t%d\t%s\n", file_name, line_num, "FOR"); break;}
                case GOTO:       {printf("%s\t%d\t%s\n", file_name, line_num, "GOTO"); break;}
                case IF:         {printf("%s\t%d\t%s\n", file_name, line_num, "IF"); break;}
                case INLINE:     {printf("%s\t%d\t%s\n", file_name, line_num, "INLINE"); break;}
                case INT:        {printf("%s\t%d\t%s\n", file_name, line_num, "INT"); break;}
                case LONG:       {printf("%s\t%d\t%s\n", file_name, line_num, "LONG"); break;}
                case REGISTER:   {printf("%s\t%d\t%s\n", file_name, line_num, "REGISTER"); break;}
                case RESTRICT:   {printf("%s\t%d\t%s\n", file_name, line_num, "RESTRICT"); break;}
                case RETURN:     {printf("%s\t%d\t%s\n", file_name, line_num, "RETURN"); break;}
                case SHORT:      {printf("%s\t%d\t%s\n", file_name, line_num, "SHORT"); break;}
                case SIGNED:     {printf("%s\t%d\t%s\n", file_name, line_num, "SIGNED"); break;}
                case SIZEOF:     {printf("%s\t%d\t%s\n", file_name, line_num, "SIZEOF"); break;}
                case STATIC:     {printf("%s\t%d\t%s\n", file_name, line_num, "STATIC"); break;}
                case STRUCT:     {printf("%s\t%d\t%s\n", file_name, line_num, "STRUCT"); break;}
                case SWITCH:     {printf("%s\t%d\t%s\n", file_name, line_num, "SWITCH"); break;}
                case TYPEDEF:    {printf("%s\t%d\t%s\n", file_name, line_num, "TYPEDEF"); break;}
                case UNION:      {printf("%s\t%d\t%s\n", file_name, line_num, "UNION"); break;}
                case UNSIGNED:   {printf("%s\t%d\t%s\n", file_name, line_num, "UNSIGNED"); break;}
                case VOID:       {printf("%s\t%d\t%s\n", file_name, line_num, "VOID"); break;}
                case VOLATILE:   {printf("%s\t%d\t%s\n", file_name, line_num, "VOLATILE"); break;}
                case WHILE:      {printf("%s\t%d\t%s\n", file_name, line_num, "WHILE"); break;}
                case _BOOL:      {printf("%s\t%d\t%s\n", file_name, line_num, "_BOOL"); break;}
                case _COMPLEX:   {printf("%s\t%d\t%s\n", file_name, line_num, "_COMPLEX"); break;}
                case _IMAGINARY: {printf("%s\t%d\t%s\n", file_name, line_num, "_IMAGINARY"); break;}

                // Identifiers
                case IDENT:   {printf("%s\t%d\t%s\t%s\n", file_name, line_num, "IDENT", yylval.ident); break;}

                // Strings
                case STRING: {
                    printf("%s\t%d\t%s\t", file_name, line_num, "STRING");
                    for (int i = 0; i < yylval.str.length; i++){
                        // printf("%hhx", yylval.str.content[i]);
                        char * tmp = to_char(yylval.str.content[i]);
                        printf("%s", tmp);
                    }
                    printf("\n");
                    break;
                }

                // Char Literal
                case CHARLIT: {printf("%s\t%d\t%s\t%s\n", file_name, line_num, "CHARLIT", to_char(yylval.charlit)); break;}

                // Special multi-byte chars
                case INDSEL:        {printf("%s\t%d\t%s\n", file_name, line_num, "INDSEL"); break;}
                case PLUSPLUS:      {printf("%s\t%d\t%s\n", file_name, line_num, "PLUSPLUS"); break;}
                case MINUSMINUS:    {printf("%s\t%d\t%s\n", file_name, line_num, "MINUSMINUS"); break;}
                case SHL:           {printf("%s\t%d\t%s\n", file_name, line_num, "SHL"); break;}
                case SHR:           {printf("%s\t%d\t%s\n", file_name, line_num, "SHR"); break;}
                case LTEQ:          {printf("%s\t%d\t%s\n", file_name, line_num, "LTEQ"); break;}
                case GTEQ:          {printf("%s\t%d\t%s\n", file_name, line_num, "GTEQ"); break;}
                case EQEQ:          {printf("%s\t%d\t%s\n", file_name, line_num, "EQEQ"); break;}
                case NOTEQ:         {printf("%s\t%d\t%s\n", file_name, line_num, "NOTEQ"); break;}
                case LOGAND:        {printf("%s\t%d\t%s\n", file_name, line_num, "LOGAND"); break;}
                case LOGOR:         {printf("%s\t%d\t%s\n", file_name, line_num, "LOGOR"); break;}
                case ELLIPSIS:      {printf("%s\t%d\t%s\n", file_name, line_num, "ELLIPSIS"); break;}
                case TIMESEQ:       {printf("%s\t%d\t%s\n", file_name, line_num, "TIMESEQ"); break;}
                case DIVEQ:         {printf("%s\t%d\t%s\n", file_name, line_num, "DIVEQ"); break;}
                case MODEQ:         {printf("%s\t%d\t%s\n", file_name, line_num, "MODEQ"); break;}
                case PLUSEQ:        {printf("%s\t%d\t%s\n", file_name, line_num, "PLUSEQ"); break;}
                case MINUSEQ:       {printf("%s\t%d\t%s\n", file_name, line_num, "MINUSEQ"); break;}
                case SHLEQ:         {printf("%s\t%d\t%s\n", file_name, line_num, "SHLEQ"); break;}
                case SHREQ:         {printf("%s\t%d\t%s\n", file_name, line_num, "SHREQ"); break;}
                case ANDEQ:         {printf("%s\t%d\t%s\n", file_name, line_num, "ANDEQ"); break;}
                case OREQ:          {printf("%s\t%d\t%s\n", file_name, line_num, "OREQ"); break;}
                case XOREQ:         {printf("%s\t%d\t%s\n", file_name, line_num, "XOREQ"); break;}

                // Number constants
                case NUMBER: {
                    printf("%s\t%d\t%s\t", file_name, line_num, "NUMBER");
                    switch (yylval.num.type){
                        case I:     {printf("%s\t%lld\t%s\n", "INTEGER", yylval.num.integer, "INT"); break;}
                        case U:     {printf("%s\t%lld\t%s\n", "INTEGER", yylval.num.integer, "UNSIGNED,INT"); break;}
                        case UL:    {printf("%s\t%lld\t%s\n", "INTEGER", yylval.num.integer, "UNSIGNED,LONG"); break;}
                        case ULL:   {printf("%s\t%lld\t%s\n", "INTEGER", yylval.num.integer, "UNSIGNED,LONGLONG"); break;}
                        case L:     {printf("%s\t%lld\t%s\n", "INTEGER", yylval.num.integer, "LONG"); break;}
                        case LL:    {printf("%s\t%lld\t%s\n", "INTEGER", yylval.num.integer, "LONGLONG"); break;}
                        case F:     {printf("%s\t%Lg\t%s\n", "REAL", yylval.num.frac, "FLOAT"); break;}
                        case D:     {printf("%s\t%Lg\t%s\n", "REAL", yylval.num.frac, "DOUBLE"); break;}
                        case LD:    {printf("%s\t%Lg\t%s\n", "REAL", yylval.num.frac, "LONGDOUBLE"); break;}
                        default: break; 
                    } break;                     
                }
                case 0 ... 255:     {printf("%s\t%d\t%c\n", file_name, line_num, t); break;}
                default: exit(2); 
            }
        }
        // printf("EOF\n");
}