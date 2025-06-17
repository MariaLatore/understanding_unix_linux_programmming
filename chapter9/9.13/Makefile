C_FLAG=-g -Wall -fsanitize=address 

smsh: smsh1.o execute.o splitline.o ifcmd.o whilecmd.o
	gcc $(C_FLAG)  $^ -o $@

smsh1.o: smsh1.c
	gcc  $(C_FLAG) -c $<

execute.o: execute.c
	gcc  $(C_FLAG) -c $<

splitline.o: splitline.c
	gcc $(C_FLAG) -c $<

ifcmd.o: ifcmd.c
	gcc $(C_FLAG) -c $<

whilecmd.o: whilecmd.c
	gcc $(C_FLAG) -c $<

clean:
	rm -rf *.o
	rm -rf smsh
