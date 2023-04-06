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

ASSIGNMENT=hw4
CXXFLAGS=-I./include -I./build -Wall -Wextra -lm -std=c99 -D _GNU_SOURCE
BUILD = build


all: $(BUILD)/parser.tab.c $(BUILD)/lex.yy.c ast.c lex_help.c sym.c type.c main.c die-util.c
	gcc $(CXXFLAGS) $^ -o build/a.out

$(BUILD)/parser.tab.c: parser.y
	mkdir -p $(BUILD)
	bison -d --report=all --file-prefix=$(BUILD)/parser $^

$(BUILD)/lex.yy.c: lexer.l $(BUILD)/parser.tab.c
	mkdir -p $(BUILD)
	flex --outfile=$(BUILD)/lex.yy.c $<

test: all
	gcc -E ptests/$(ASSIGNMENT)/$(FILE).c | ./$(BUILD)/a.out >ptests/$(ASSIGNMENT)/$(ASSIGNMENT)_output/$(FILE).out 2>ptests/$(ASSIGNMENT)/$(ASSIGNMENT)_output/$(FILE).err

print: all
	gcc -E ptests/$(ASSIGNMENT)/$(FILE).c | ./$(BUILD)/a.out

clean: 
	rm build/*
