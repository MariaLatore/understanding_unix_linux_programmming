C_FLAG= -g -Wall -fsanitize=address 
OBJS=smsh1.o execute.o splitline.o ifcmd.o whilecmd.o builtin.o varlib.o 

smsh: $(OBJS)
	gcc $(C_FLAG) $^ -o $@
$(OBJS): %.o:%.c
	gcc $(C_FLAG) -c $<

clean:
	rm -rf $(OBJS)
