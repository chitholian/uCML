CC		= gcc
LEX		= flex
BISON		= bison


PROGRAM		= parser


parser: lexer.c parser.c
	$(CC) -o $(PROGRAM) lexer.c parser.c
	
lexer.c: lexer.l
	$(LEX) -o lexer.c lexer.l
	
parser.c: parser.y
	$(BISON) -d -o parser.c parser.y
	
clean:
	-rm -rf parser parser.c parser.h lexer.c
