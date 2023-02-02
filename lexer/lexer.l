%option noyywrap
%{
#include "tokens-manual.h"

int yylval;
char * file_name;
int line_num = 1;
char * ident;


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

[_A-Za-z]+[A-Za-z_0-9]*                                     {ident = strdup(yytext); return IDENT;}

[()\[\]{},\.;:\?'"!\|\/\^&\*\+\-><~_\$%#=]                  {
                                                                char* tmp_special = strdup(yytext);
                                                                return *tmp_special;
                                                            }

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
                case IDENT:      {printf("%s\t%d\t%s\t%s\n", file_name, line_num, "IDENT", ident); break;}

                // Int
                // If its a single character, this should be the default
                // default: {printf("%s\t%d\t%c\n", file_name, line_num, t); break;}
                // case AUTO
            }
        }
        // printf("EOF\n");
}

