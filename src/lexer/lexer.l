%option noinput nounput noyywrap nodefault
%{
// #include "lexer/lexer.h"
#include "parser.tab.h"


YYSTYPE yylval;

// Global for line number
int line_num = 1;

%}
%%
#[ \t][0-9]+[ \t]\"[a-zA-Z0-9\/\-<>._ ]+\"[ \t]*[0-9]*\n      {file_start(yytext);}

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
\<=         {return LTEQ;}
\>=         {return GTEQ;}
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

\/\/.*\n                {line_num++;}

\/\*.*\*\/             

\n                      {line_num++;}
.                       {fprintf(stderr,"%s\t%d\tError: unknown token %s\n",file_name, line_num, yytext);}


%%
