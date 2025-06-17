SRC_FILES:=controlflow.c execute.c process.c smsh3.c splitline.c varlib.c builtin.c
OBJ_FILES:=${SRC_FILES:%.c=%.o}

smsh3: ${OBJ_FILES}
	gcc -g -Wall $^ -o $@

${OBJ_FILES}: %.o:%.c
	gcc -g -Wall -c $<

clean:
	rm -rf ${OBJ_FILES}
