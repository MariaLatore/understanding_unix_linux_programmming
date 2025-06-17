SRC_FILES:=controlflow.c execute.c process.c smsh2.c splitline.c
OBJ_FILES:=${SRC_FILES:%.c=%.o}

smsh2: ${OBJ_FILES}
	gcc -g -Wall $^ -o $@

${OBJ_FILES}: %.o:%.c
	gcc -g -Wall -c $<

clean:
	rm -rf ${OBJ_FILES}
	rm smsh2
	rm -rf *~
