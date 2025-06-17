smsh: smsh1.o execute.o splitline.o
	gcc -Wall $^ -o $@

smsh1.o: smsh1.c
	gcc -Wall -c $<

execute.o: execute.c
	gcc -Wall -c $<

splitline.o: splitline.c
	gcc -Wall -c $<

clean:
	rm -rf *.o
	rm -rf smsh
