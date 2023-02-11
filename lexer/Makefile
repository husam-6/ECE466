all:
	flex lexer.l
	gcc -ggdb -Wall -Wextra -fsanitize=undefined lex.yy.c

test: all
	gcc -E ltests/*.c | ./a.out >test.out 2>test.err