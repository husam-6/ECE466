// yylval
typedef union {
          char *string_literal;
          int integer;
} YYSTYPE;

extern YYSTYPE yylval;

// Globals to store file name and line number
char * file_name;
int line_num = 1;