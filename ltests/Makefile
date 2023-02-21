vpath %.h ../include
vpath %.h ../build
vpath %.c ../build
vpath %.c ../src
vpath %.y ../src
vpath %.l ../src
CXXFLAGS=-I../include -I../build -ggdb -Wall -Wextra -fsanitize=undefined

all: lex.yy.c
	gcc $(CXXFLAGS) $< 

lex.yy.c: lexer.l lexer.h
	flex $<

test: all
	gcc -E samples/*.c | ./a.out >test.out 2>test.err