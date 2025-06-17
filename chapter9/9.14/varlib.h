void VLlist ();
//return char* should be freed after use
char *VLlookup (char *);
int VLstore (char *, char *);
int VLexport (char *);
char **VLtable2environ ();
int VLenviron2table (char *env[]);
