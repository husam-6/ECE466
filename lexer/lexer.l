%option noyywrap
%{
#define TEST 1

int yylval;
char * file_name;
int line_num = 1;

char * token_name;
char * semantic_meaning;

%}
%%
#[ \t][0-9]+[ \t]\"[a-zA-Z0-9\/]+.c\"   {
                                            // Extract line number and file name
                                            char* str_mod = strdup(yytext);
                                            char* token = strtok(str_mod, " ");
                                            
                                            // Split file and loop through each element
                                            int i = 0; 
                                            while( token != NULL ) {
                                                if (i == 1){
                                                    line_num = atoi(token);
                                                }
                                                else if (i == 2){
                                                    file_name = token+1;
                                                    file_name[strlen(file_name)-1] = '\0'; 
                                                }
                                                token = strtok(NULL, " ");
                                                i++; 
                                            }
                                            return TEST;
                                        }
.                                       {fprintf(stderr,"Error: unknown token %s\n",yytext);}
%%


int main() {
    int t;
    while((t=yylex()))
        {
        switch(t) {
            case TEST: printf("%s \t %d\n", file_name, line_num); break;
            }
        }
        printf("EOF\n");
}

