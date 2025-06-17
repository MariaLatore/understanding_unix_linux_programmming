smsh: smsh1.o execute.o splitline.o
	gcc -g -Wall $^ -o $@

smsh1.o: smsh1.c
	gcc -g -Wall -c $<

execute.o: execute.c
	gcc -g -Wall -c $<

splitline.o: splitline.c
	gcc -g -Wall -c $<

clean:
	rm -rf *.o
	rm -rf smsh
