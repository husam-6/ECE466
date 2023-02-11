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

// yylval
typedef union {
          struct string_literal str;
          char charlit;
          char *ident;
          struct number num; 
} YYSTYPE;

extern YYSTYPE yylval;

// Globals to store file name and line number
char * file_name;
int line_num = 1;

// To store running string
char buf[2048];
int i = 0;
