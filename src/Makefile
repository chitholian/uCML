UCaml: UCaml.l UCaml.y
	bison -d UCaml.y
	flex UCaml.l
	cc -o $@ UCaml.tab.c lex.yy.c -lfl
