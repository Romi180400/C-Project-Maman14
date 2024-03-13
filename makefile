CFLAGS = -ansi -g -pedantic -Wall -Wextra -fsanitize=address

assembler:lexer.o assembler.o back.o translation_unit.o utils.o preproc.o
	gcc $(CFLAGS) $^ -o $@

lexer.o: lexer/lexer.c lexer/lexer.h
	gcc $(CFLAGS)  -c $<  

assembler.o: assembler.c lexer/lexer.h preproc.h translation_unit.h
	gcc $(CFLAGS)  -c $< 

back.o: back.c translation_unit.h utils.h
	gcc $(CFLAGS)  -c $< 

main.o: main.c translation_unit.h
	gcc $(CFLAGS)  -c $<  

translation_unit.o: translation_unit.c translation_unit.h
	gcc $(CFLAGS)  -c $<  

utils.o: utils.c utils.h
	gcc $(CFLAGS)  -c $< 

preproc.o: preproc.c
	gcc $(CFLAGS)  -c $< 

clean:
	rm -f *.o *.out
	rm -f assembler
	rm -f mmn14.zip
	rm -f tests/*.ob tests/*.ent tests/*.ext tests/*.am

zip: clean
	zip -r mmn14.zip *