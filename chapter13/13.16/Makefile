SRC_FILES:=controlflow.c execute.c process.c smsh4.c splitline.c varlib.c builtin.c
SRC_FILES2:=license/dgram.c license/lcnt_funcs2.c
OBJ_FILES:=${SRC_FILES:%.c=%.o}
OBJ_FILES2:=${SRC_FILES2:license/%.c=%.o}
C_FLAG:=-g -Wall -Ilicense

smsh4: ${OBJ_FILES} $(OBJ_FILES2)
	gcc $(C_FLAG) $^ -o $@

${OBJ_FILES}: %.o:%.c
	gcc $(C_FLAG) -c $<

${OBJ_FILES2}: %.o:license/%.c
	gcc $(C_FLAG) -c $<

clean:
	rm -rf ${OBJ_FILES} $(OBJ_FILES2)

debug:
	echo "obj files $(OBJ_FILES)"
	echo "src files2 $(SRC_FILES2)"
	echo "obj files2 $(OBJ_FILES2)"
