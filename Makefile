vpath %.tab.h build
vpath %.tab.c build
vpath %.yy.c build
vpath %.h include
vpath %.c src
vpath %.c src/parser
vpath %.c src/lexer
vpath %.y src/parser
vpath %.l src/lexer
CXXFLAGS=-I./include -I./build -Wall -Wextra
BUILD = build

all: parser.tab.c lex.yy.c ast.c lex_help.c sym.c type.c
	gcc $(CXXFLAGS) $^ -ll -o build/a.out

$(BUILD)/parser.tab.c: parser.y
	bison -d --report=all --file-prefix=$(BUILD)/parser $^

$(BUILD)/lex.yy.c: lexer.l parser.tab.c
	flex --outfile=$(BUILD)/lex.yy.c $<

test: all
	gcc -E ptests/*.c | ./$(BUILD)/a.out >ptests/test.out 2>ptests/test.err

testout: all
	gcc -E ptests/*.c | ./$(BUILD)/a.out

clean: 
	rm build/*