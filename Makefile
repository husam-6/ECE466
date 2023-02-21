all: parser.tab.c lex.yy.c 
	gcc lex.yy.c parser.tab.c -ll

parser.tab.c: ./parser/parser.y
	bison -d --report=all ./parser/parser.y

lex.yy.c: ./lexer/lexer.l parser.tab.c
	flex ./lexer/lexer.l


clean: 
	rm lex.yy.c
	rm parser.tab.c
	rm parser.tab.h
	rm a.out