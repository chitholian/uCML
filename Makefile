SRC			= src
OUT			= build
CC 			= gcc
CFLAGS 			= -Wall -g
LEX 			= flex
LEXER			= uCMLexer

LEX_SOURCE		= $(SRC)/lexer.l
LEX_OUT			= $(OUT)/lexer.c


$(OUT): $(LEX_SOURCE)
	mkdir -p $(OUT)
	$(LEX) -o $(LEX_OUT) $(LEX_SOURCE)
	$(CC) $(CFLAGS) -o $(OUT)/$(LEXER) $(LEX_OUT)
	
	
help:
	@echo To compile and build run:
	@echo "	make build"
	@echo To clean up build folder run:
	@echo "	make clean"
	@echo To test lexical anlyzer run:
	@echo "	./build/uCMLexer [INPUT FILE]"
	@echo To see this help run:
	@echo "	make help"

clean:
	-rm -rf $(OUT)
