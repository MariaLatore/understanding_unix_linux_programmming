#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAXVARS 100

struct var
{
  char *str;			/*name=val string */
  int global;			/* a boolean */
};
static struct var tab[MAXVARS] = { {.str = NULL,.global = 0,} };

void
VLlist ()
{
  int i = 0;
  while (i < MAXVARS && tab[i].str != NULL)
    {
      printf ("%s, global:%d\n", tab[i].str, tab[i].global);
      i++;
    }
}

char *
VLookup (char *var)
{

  int i = 0;
  while (i < MAXVARS && tab[i].str != NULL)
    {
      if (strstr (tab[i].str, var) != NULL)
	{
	  char *ret_str = (char *) malloc (strlen (tab[i].str) + 1);
	  if (ret_str == NULL)
	    {
	      puts ("error malloc\n");
	      return NULL;
	    }
	  strcpy (ret_str, tab[i].str);
	  return ret_str;
	}
      i++;
    }

  return NULL;
}



int
VLstore (char *var, char *val)
{
  int i = 0;
  char *cp = NULL;
  while (i < MAXVARS && tab[i].str != NULL)
    {
      if (((cp = strstr (tab[i].str, var)) != NULL)
	  && (*(cp + strlen (var)) == '='))
	break;
      i++;
    }

  if (i == MAXVARS)
    {
      puts ("VLlist space is not enough!\n");
      return -1;
    }

  if (tab[i].str)
    free (tab[i].str);

  tab[i].str =
    (char *) malloc (strlen (var) + strlen ("=") + strlen (val) + 1);
  if (!tab[i].str)
    {
      puts ("malloc error\n");
      return -1;
    }

  sprintf (tab[i].str, "%s=%s", var, val);
  return 0;
}

int
VLexport (char *var)
{
  int i = 0;
  char *cp = NULL;
  while (i < MAXVARS && tab[i].str != NULL)
    {
      if (((cp = strstr (tab[i].str, var)) != NULL)
	  && (*(cp + strlen (var)) == '='))
	{
	  tab[i].global = 1;
	  return 0;
	}
      i++;
    }

  printf ("cannot find var %s\n", var);
  return -1;
}
