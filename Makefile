# Parameters
CXXFLAGS=-I./include -I./build -Wall -Wextra -fsanitize=undefined -ggdb
BUILD = build
VPATH = src/lexer/:src/parser/:src/quads/:src

all: $(BUILD)/parser.tab.o $(BUILD)/lex.yy.o $(BUILD)/ast.o $(BUILD)/lex_help.o $(BUILD)/sym.o $(BUILD)/type.o $(BUILD)/main.o $(BUILD)/die-util.o
	gcc $(CXXFLAGS) $^ -o $(BUILD)/a.out

# Specific rule for parser (bison)
$(BUILD)/parser.tab.o: $(BUILD)/parser.tab.c
	gcc -c $^ $(CXXFLAGS) -o $@

$(BUILD)/parser.tab.c: parser.y
	mkdir -p $(BUILD)
	bison -d --report=all --file-prefix=$(BUILD)/parser $^

# Specific rules for lexer (flex)
$(BUILD)/lex.yy.o: $(BUILD)/lex.yy.c
	gcc -c $^ $(CXXFLAGS) -o $@

$(BUILD)/lex.yy.c: lexer.l $(BUILD)/parser.tab.c
	mkdir -p $(BUILD)
	flex --outfile=$(BUILD)/lex.yy.c $<

# For any generic .c file
$(BUILD)/%.o: %.c
	gcc -c $< $(CXXFLAGS) -o $@

clean: 
	rm build/*