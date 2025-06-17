SRC_FILES:=controlflow.c execute.c process.c smsh2.c splitline.c
OBJ_FILES:=${SRC_FILES:%.c=%.o}

smsh2: ${OBJ_FILES}
	gcc -Wall $^ -o $@

${OBJ_FILES}: %.o:%.c
	gcc -Wall -c $<

clean:
	rm -rf ${OBJ_FILES}
