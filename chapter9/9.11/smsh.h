#define YES 1
#define NO 0
#define MBLOCK 100

typedef struct
{
  char **condition;
  char ***loopblock;
} WhileStruct;

typedef struct
{
  char **condition;
  char ***then_list;
  char ***else_list;
} IfStructure;


char *next_cmd ();
char **splitline (char *);
void freelist (char **);
void *emalloc (size_t);
void *erealloc (void *, size_t);
int execute (char **);
void fatal (char *, char *, int);
char *newstr (char *, int);

void free_if_structure (IfStructure *);
void execute_if_cmdlist (IfStructure *);
int generate_if_cmdlist (char ***, IfStructure **);
char **newstrlist (char **);
void do_if ();

char *next_valid_cmdline ();
void free_while_structure (WhileStruct *);
void execute_while_cmdlist (WhileStruct *);
int generate_while_cmdlist (char ***, WhileStruct **);
void do_while ();
