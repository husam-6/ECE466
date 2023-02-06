all:
	flex lexer.l
	gcc lex.yy.c

test: all
	gcc -E samples/*.c | ./a.out >test.out 2>test.err