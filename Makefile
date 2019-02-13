SRC			= src
OUT			= build
TESTS			= tests
CC 			= gcc
CFLAGS 			= -Wall -g
LEX 			= flex
LEXER			= uCMLexer

LEX_SOURCE		= $(SRC)/lexer.l
LEX_OUT			= $(OUT)/lexer.c

TEST_LEXER_IN		= $(TESTS)/lexer.input

$(OUT): $(LEX_SOURCE)
	mkdir -p $(OUT)
	$(LEX) -o $(LEX_OUT) $(LEX_SOURCE)
	$(CC) $(CFLAGS) -o $(OUT)/$(LEXER) $(LEX_OUT)
	
	
help:
	@echo Compile:
	@echo "	make $(OUT)"
	@echo 'It will create an executable (the lexical analyzer) "$(LEXER)" in "$(OUT)" folder.'
	@echo
	@echo Delete $(OUT) directory:
	@echo "	make clean"
	@echo
	@echo Test lexical analyzer:
	@echo "	make test-lexer [TEST_LEXER_IN=file]"
	@echo
	@echo See this help:
	@echo "	make help"
	@echo
	
	
test-lexer: $(OUT)
	@echo "Trying to test lexical analyzer using input file $(TEST_LEXER_IN)"
	@$(OUT)/$(LEXER) $(TEST_LEXER_IN)

clean:
	-rm -rf $(OUT)
