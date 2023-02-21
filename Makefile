vpath %.tab.h build
vpath %.tab.c build
vpath %.yy.c build
vpath %.h include
vpath %.c src
vpath %.y src
vpath %.l src
CXXFLAGS=-I./include -I./build
BUILD = build

all: parser.tab.c lex.yy.c 
	gcc $(CXXFLAGS) $^ -ll -o build/a.out

$(BUILD)/parser.tab.c: parser.y
	bison -d --report=all --file-prefix=build/parser $^

$(BUILD)/lex.yy.c: lexer.l parser.tab.c
	flex --outfile=build/lex.yy.c $<

clean: 
	rm build/*