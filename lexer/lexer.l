%option noyywrap
%{
#include "tokens-manual.h"
#include "lexer.h"

YYSTYPE yylval;

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

[_A-Za-z]+[A-Za-z_0-9]*                                     {yylval.ident = strdup(yytext); return IDENT;}

[L]?["]([^"\\\n]|\\.|\\\n)*["]                              {yylval.string_literal = strdup(yytext); return STRING;}


[L]?['](\\[\\\'\"\?abfnrtv]|[^\\]|\\[0-7][0-7]?[0-7]?|\\x[0-9a-fA-F][0-9a-fA-F]?)[']                 {yylval.charlit = strdup(yytext); return CHARLIT;}
[L]?['].+[']                                                                                       {fprintf(stderr, "Warning: Invalid Char Sequence");}

[0][0-7]*                                          {yylval.num.integer = octal_to_dec(atoi(yytext)); yylval.num.type = I; return NUMBER;}
[0][0-7]*[uU]                                      {yylval.num.integer = octal_to_dec(atoi(yytext)); yylval.num.type = U; return NUMBER;}
[0][0-7]*[uU][lL]|[0][0-7]*[lL][uU]                {yylval.num.integer = octal_to_dec(atoi(yytext)); yylval.num.type = UL; return NUMBER;}
[0][0-7]*[uU][lL][lL]|[0][0-7]*[lL][lL][uU]        {yylval.num.integer = octal_to_dec(atoi(yytext)); yylval.num.type = ULL; return NUMBER;}
[0][0-7]*[lL]                                      {yylval.num.integer = octal_to_dec(atoi(yytext)); yylval.num.type = L; return NUMBER;}
[0][0-7]*[lL][lL]                                  {yylval.num.integer = octal_to_dec(atoi(yytext)); yylval.num.type = LL; return NUMBER;}

[1-9][0-9]*                                        {yylval.num.integer = atoi(yytext); yylval.num.type = I; return NUMBER;}
[1-9][0-9]*[uU]                                    {yylval.num.integer = atoi(yytext); yylval.num.type = U; return NUMBER;}
[1-9][0-9]*[uU][lL]|[1-9][0-9]*[lL][uU]                        {yylval.num.integer = atoi(yytext); yylval.num.type = UL; return NUMBER;}
[1-9][0-9]*[uU][lL][lL]|[1-9][0-9]*[lL][lL][uU]                      {yylval.num.integer = atoi(yytext); yylval.num.type = ULL; return NUMBER;}
[1-9][0-9]*[lL]                                    {yylval.num.integer = atoi(yytext); yylval.num.type = L; return NUMBER;}
[1-9][0-9]*[lL][lL]                                {yylval.num.integer = atoi(yytext); yylval.num.type = LL; return NUMBER;}

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
\.\.\.         {return ELLIPSIS;}
\*=         {return TIMESEQ;}
\/=         {return DIVEQ;}
%=          {return MODEQ;}
\+=         {return PLUSEQ;}
-=          {return MINUSEQ;}
&=          {return ANDEQ;}
\|=         {return OREQ;}
\^=         {return XOREQ;}


[()\[\]{},\.;:\?'"!\|\/\^&\*\+\-><~_\$%#=]  {return yytext[0];}

\n                      {line_num++;}
.                       {fprintf(stderr,"Error: unknown token %s\n",yytext);}


%%



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
                case STRING: {printf("%s\t%d\t%s\t%s\n", file_name, line_num, "STRING", yylval.string_literal); break;}

                // Char Literal
                case CHARLIT: {printf("%s\t%d\t%s\t%s\n", file_name, line_num, "CHARLIT", yylval.charlit); break;}

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
                        case I: {printf("%s\t%llu\t%s\n", "INTEGER", yylval.num.integer, "INT"); break;}
                        case U: {printf("%s\t%llu\t%s\n", "INTEGER", yylval.num.integer, "UNSIGNED,INT"); break;}
                        case UL: {printf("%s\t%llu\t%s\n", "INTEGER", yylval.num.integer, "UNSIGNED,LONG"); break;}
                        case ULL: {printf("%s\t%llu\t%s\n", "INTEGER", yylval.num.integer, "UNSIGNED,LONGLONG"); break;}
                        case L: {printf("%s\t%llu\t%s\n", "INTEGER", yylval.num.integer, "LONG"); break;}
                        case LL: {printf("%s\t%llu\t%s\n", "INTEGER", yylval.num.integer, "LONGLONG"); break;}
                        case F: {printf("%s\t%.1Lf\t%s\n", "REAL", yylval.num.frac, "FLOAT"); break;}
                        case D: {printf("%s\t%.1Lf\t%s\n", "REAL", yylval.num.frac, "DOUBLE"); break;}
                        case LD: {printf("%s\t%.1Lf\t%s\n", "REAL", yylval.num.frac, "LONGDOUBLE"); break;}
                        default: break; 
                    } break;                     
                }
                case 0 ... 255: {printf("%s\t%d\t%c\n", file_name, line_num, t); break;}
                default: exit(2); 
            }
        }
        // printf("EOF\n");
}

