%option noinput nounput noyywrap nodefault
%{
#include "lexer/lexer.h"
#include "expr.tab.h"
#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "math.h"

YYSTYPE yylval;

// Function to parse a directive indicating a new file 
void file_start(char* yytext);

// Function to handle hex fractions (with p-2 at the end for example)
long double hex_frac(char *yytext);

// Convert inputted string for storage
char to_bytes(char * inp, int * jump);

// Convert back to string
char * to_char(unsigned char inp);

%}
%%
#[ \t][0-9]+[ \t]\"[a-zA-Z0-9\/\-<>. ]+\"[ \t]*[0-9]*\n      {file_start(yytext);}

auto                    {return AUTO;}
break                   {return BREAK;}
case                    {return CASE;}
char                    {return CHAR;}
const                   {return CONST;}
continue                {return CONTINUE;}
default                 {return DEFAULT;}
do                      {return DO;}
double                  {return DOUBLE;}
else                    {return ELSE;}
enum                    {return ENUM;}
extern                  {return EXTERN;}
float                   {return FLOAT;}
for                     {return FOR;}
goto                    {return GOTO;}
if                      {return IF;}
inline                  {return INLINE;}
int                     {return INT;}
long                    {return LONG;}
register                {return REGISTER;}
restrict                {return RESTRICT;}
return                  {return RETURN;}
short                   {return SHORT;}
signed                  {return SIGNED;}
sizeof                  {return SIZEOF;}
static                  {return STATIC;}
struct                  {return STRUCT;}
switch                  {return SWITCH;}
typedef                 {return TYPEDEF;}
union                   {return UNION;}
unsigned                {return UNSIGNED;}
void                    {return VOID;} 
volatile                {return VOLATILE;}
while                   {return WHILE;}
_Bool                   {return _BOOL;}
_Complex                {return _COMPLEX;}
_Imaginary              {return _IMAGINARY;}

[_A-Za-z]+[A-Za-z_0-9]*                             {yylval.ident = strdup(yytext); return IDENT;}


[L]?["].*\\x[^0-9a-fA-F].*["]|[L]?["].*\\x["]                   {fprintf(stderr, "Error: Invalid Hex Sequence\n");}
[L]?["].*\\x[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F].*["]              {fprintf(stderr, "Warning: Invalid Hex Sequence Out of Bounds\n");}
[L]?["].*\\[^\\\'\"\?abfnrtvx0-7].*["]                          {fprintf(stderr, "Error: Invalid String\n");}
[L]?["](\\\"|[^"\n]|\\\n)*["]                                      {
                                                                    yytext = yytext + 1; yytext[strlen(yytext)-1] = '\0';
                                                                    char output[strlen(yytext)+1]; 
                                                                    int j = 0; 
                                                                    int jump = 0; 
                                                                    // printf("YYTEXT = %s\n", yytext);
                                                                    for (size_t i = 0; i < strlen(yytext);){
                                                                        output[j] = to_bytes(&yytext[i], &jump);
                                                                        // printf("CHAR = %hhx\n", output[j]);
                                                                        i+=jump; 
                                                                        j++;
                                                                    } 
                                                                    output[j] = '\0';

                                                                    yylval.str.content = malloc(j+1*sizeof(char));
                                                                    memcpy(yylval.str.content, output, j+1);
                                                                    yylval.str.length = j;
                                                                    
                                                                    return STRING;
                                                                }



[L]?['](\\[\\\'\"\?abfnrtv]|[^\\]+|\\[0-7][0-7]?[0-7]?|\\x[0-9a-fA-F][0-9a-fA-F]?)[']   {
                                                                                            yytext = yytext + 1; yytext[strlen(yytext)-1] = '\0'; 
                                                                                            int jump = 0; 
                                                                                            yylval.charlit = to_bytes(yytext, &jump);
                                                                                            if ((size_t) jump > strlen(yytext)) {fprintf(stderr, "ERROR: IDK\n"); exit(2);}
                                                                                            return CHARLIT;
                                                                                        }
[L]?['].+[']                                       {fprintf(stderr, "Error: Invalid Char Sequence\n"); exit(2);}

[0][0-7]*                                          {yylval.num.integer = strtoull(yytext, NULL, 8); yylval.num.type = I; return NUMBER;}
[0][0-7]*[uU]                                      {yylval.num.integer = strtoull(yytext, NULL, 8); yylval.num.type = U; return NUMBER;}
[0][0-7]*[uU][lL]|[0][0-7]*[lL][uU]                {yylval.num.integer = strtoull(yytext, NULL, 8); yylval.num.type = UL; return NUMBER;}
[0][0-7]*[uU][lL][lL]|[0][0-7]*[lL][lL][uU]        {yylval.num.integer = strtoull(yytext, NULL, 8); yylval.num.type = ULL; return NUMBER;}
[0][0-7]*[lL]                                      {yylval.num.integer = strtoull(yytext, NULL, 8); yylval.num.type = L; return NUMBER;}
[0][0-7]*[lL][lL]                                  {yylval.num.integer = strtoull(yytext, NULL, 8); yylval.num.type = LL; return NUMBER;}

[1-9][0-9]*                                                         {yylval.num.integer = atoi(yytext); yylval.num.type = I; return NUMBER;}
[1-9][0-9]*[uU]                                                     {yylval.num.integer = atoi(yytext); yylval.num.type = U; return NUMBER;}
[1-9][0-9]*[uU][lL]|[1-9][0-9]*[lL][uU]                             {yylval.num.integer = atoi(yytext); yylval.num.type = UL; return NUMBER;}
[1-9][0-9]*[uU][lL][lL]|[1-9][0-9]*[lL][lL][uU]                     {yylval.num.integer = atoi(yytext); yylval.num.type = ULL; return NUMBER;}
[1-9][0-9]*[lL]                                                     {yylval.num.integer = atoi(yytext); yylval.num.type = L; return NUMBER;}
[1-9][0-9]*[lL][lL]                                                 {yylval.num.integer = atoi(yytext); yylval.num.type = LL; return NUMBER;}

[0-9]+[.][0-9]*|[0-9]*[.][0-9]+                                     {yylval.num.frac = strtod(yytext, NULL); yylval.num.type = D; return NUMBER;}
[0-9]+[.][0-9]*[lL]|[0-9]*[.][0-9]+[lL]                             {yylval.num.frac = strtod(yytext, NULL); yylval.num.type = LD; return NUMBER;}
[0-9]+[.][0-9]*[fF]|[0-9]+[.][0-9]*[fF]                             {yylval.num.frac = strtod(yytext, NULL); yylval.num.type = F; return NUMBER;}
([0-9]+[.][0-9]*|[0-9]+[.][0-9]*|[0-9]+)[eE][+-]?[0-9]+             {yylval.num.frac = strtod(yytext, NULL); yylval.num.type = D; return NUMBER;}
([0-9]+[.][0-9]*|[0-9]+[.][0-9]*|[0-9]+)[eE][+-]?[0-9]+[fF]         {yylval.num.frac = strtod(yytext, NULL); yylval.num.type = F; return NUMBER;}
([0-9]+[.][0-9]*|[0-9]+[.][0-9]*|[0-9]+)[eE][+-]?[0-9]+[LD]         {yylval.num.frac = strtod(yytext, NULL); yylval.num.type = LD; return NUMBER;}

0[xX][0-9a-fA-F]+                                                   {yylval.num.integer = strtoull(yytext, NULL, 16); yylval.num.type = I; return NUMBER;}
0[xX][0-9a-fA-F]+[uU]                                               {yylval.num.integer = strtoull(yytext, NULL, 16); yylval.num.type = U; return NUMBER;}
0[xX][0-9a-fA-F]+[uU][lL]|0[xX][0-9a-fA-F]+[lL][uU]                 {yylval.num.integer = strtoull(yytext, NULL, 16); yylval.num.type = UL; return NUMBER;}
0[xX][0-9a-fA-F]+[lL]                                               {yylval.num.integer = strtoull(yytext, NULL, 16); yylval.num.type = L; return NUMBER;}
0[xX][0-9a-fA-F]+[lL][lL]                                           {yylval.num.integer = strtoull(yytext, NULL, 16); yylval.num.type = LL; return NUMBER;}
0[xX][0-9a-fA-F]+[uU][lL][lL]|0[xX][0-9a-fA-F]+[lL][lL][uU]         {yylval.num.integer = strtoull(yytext, NULL, 16); yylval.num.type = ULL; return NUMBER;}

0[xX][0-9a-fA-F]+[pP][+-]?[0-9]+            {yylval.num.frac = hex_frac(yytext); yylval.num.type = D; return NUMBER;}
0[xX][0-9a-fA-F]+[pP][+-]?[0-9]+[lL]        {yylval.num.frac = hex_frac(yytext);yylval.num.type = LD; return NUMBER;}
0[xX][0-9a-fA-F]+[pP][+-]?[0-9]+[fF]        {yylval.num.frac = hex_frac(yytext);yylval.num.type = F; return NUMBER;}

-\>         {return INDSEL;}
\+\+        {return PLUSPLUS;}
--          {return MINUSMINUS;}
\<\<=       {return SHLEQ;}
\>\>=       {return SHREQ;}
\<\<        {return SHL;}
\>\>        {return SHR;}
\>=         {return LTEQ;}
\<=         {return GTEQ;}
==          {return EQEQ;}
!=          {return NOTEQ;}
&&          {return LOGAND;}
\|\|        {return LOGOR;}
\.\.\.      {return ELLIPSIS;}
\*=         {return TIMESEQ;}
\/=         {return DIVEQ;}
%=          {return MODEQ;}
\+=         {return PLUSEQ;}
-=          {return MINUSEQ;}
&=          {return ANDEQ;}
\|=         {return OREQ;}
\^=         {return XOREQ;}


[()\[\]{},\.;:\?'"!\|\/\^&\*\+\-><~_\$%#=]  {return yytext[0];}

[ ]*                    

\/\/.*\n                

\/\*.*\*\/             

\n                      {line_num++;}
.                       {fprintf(stderr,"%s\t%d\tError: unknown token %s\n",file_name, line_num, yytext);}


%%

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