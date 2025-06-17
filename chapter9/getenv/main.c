#include<stdlib.h>
#include<stdio.h>
#include<string.h>
int
main ()
{
  char *cp = getenv ("LANG");
  if (cp != NULL && strcmp (cp, "fr") == 0)
    printf ("Bonjour, cp=%s\n", cp);
  else
    printf ("Hello, cp=%s\n", cp);
  return 0;
}
