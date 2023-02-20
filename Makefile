all: expr.tab.c lex.yy.c 
	gcc expr.tab.c lex.yy.c -ll

expr.tab.c: ./parser/expr.y
	bison -d --report=all ./parser/expr.y

lex.yy.c: ./lexer/lexer.l expr.tab.c
	flex ./lexer/lexer.l


clean: 
	rm lex.yy.c
	rm expr.tab.c
	rm expr.tab.h
	rm a.out