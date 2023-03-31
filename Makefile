vpath %.tab.h build
vpath %.tab.c build
vpath %.yy.c build
vpath %.h include
vpath %.c src
vpath %.c src/parser
vpath %.c src/lexer
vpath %.y src/parser
vpath %.l src/lexer
FILE=ptest
CXXFLAGS=-I./include -I./build -Wall -Wextra -fsanitize=undefined
BUILD = build

all: parser.tab.c lex.yy.c ast.c lex_help.c sym.c type.c main.c die-util.c
	gcc $(CXXFLAGS) $^ -ll -o build/a.out

$(BUILD)/parser.tab.c: parser.y
	bison -d --report=all --file-prefix=$(BUILD)/parser $^

$(BUILD)/lex.yy.c: lexer.l parser.tab.c
	flex --outfile=$(BUILD)/lex.yy.c $<

test: all
	gcc -E ptests/$(FILE).c | ./$(BUILD)/a.out >ptests/output/$(FILE).out 2>ptests/output/$(FILE).err

print: all
	gcc -E ptests/$(FILE).c | ./$(BUILD)/a.out

clean: 
	rm build/*