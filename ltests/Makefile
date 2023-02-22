vpath %.h ../include
vpath %.h ../build
vpath %.c ../build
vpath %.c ../src
vpath %.y ../src
vpath %.l ../src
CXXFLAGS=-I../include -I../build -ggdb -Wall -Wextra -fsanitize=undefined
BUILD=build

all: lex.yy.c lex_help.c lex_tester.c
	gcc $(CXXFLAGS) $^ 

$(BUILD)/lex.yy.c: lexer.l lexer.h
	flex $< --outfile=$(BUILD)/lex.yy.c

test: all
	gcc -E samples/*.c | ./a.out >test.out 2>test.err