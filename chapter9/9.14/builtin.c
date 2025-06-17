/* builtin.c
 * contains the switch and the functions for builtin commands
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "smsh.h"
#include "varlib.h"

int assign (char *);
int okname (char *);
int
builtin_command (char **args, int *resultp)
/*
 * purpose: run a builtin command
 * returns: 1 if args[0] is built-in 0 if not
 * details: test args[0] against all known built-ins. Call functions
 */
{
  int rv = 0;
  if (strcmp (args[0], "set") == 0)
    {				//'set' command?
      VLlist ();
      *resultp = 0;
      rv = 1;
    }
  else if (strchr (args[0], '=') != NULL)
    {				//assignment cmd
      *resultp = assign (args[0]);
      if (*resultp != -1)	//x-y=123 not ok
	rv = 1;
      char *find = VLlookup ("PROMPT");
      if (strcmp (find, "") != 0)
	prompt = find;
    }
  else if (strcmp (args[0], "export") == 0)
    {
      if (args[1] != NULL && okname (args[1]))
	*resultp = VLexport (args[1]);
      else
	*resultp = 1;
      rv = 1;
    }
  return (last_cmd_result = rv);
}

int
assign (char *str)
/*
 * purpose: execute name=val AND ensure that name is legal
 * returns: -1 for illegal lval, or result of VLstore
 * warning: modifies the string, but restores it to normal
 */
{
  char *cp;
  int rv;
  char *val_start = NULL;
  char *val_end = NULL;

  cp = strchr (str, '=');
  *cp = '\0';
  val_start = cp + 1;
  val_end = val_start + strlen (val_start) - 1;
  if (*val_start == '"' && *val_end == '"')
    {
      val_start++;
      val_end--;
    }
  char *val = (char *) emalloc (val_end - val_start + 2);
  memset (val, 0, val_end - val_start + 2);
  memcpy (val, val_start, val_end - val_start + 1);

  rv = (okname (str) ? VLstore (str, val) : -1);
  *cp = '=';
  free (val);
  return rv;
}

int
okname (char *str)
/*
 * purpose: determines if a string is a legal variable name
 * returns: 0 for no, 1 for yes
 */
{
  char *cp;
  for (cp = str; *cp; cp++)
    {
      if ((isdigit (*cp) && cp == str) || !(isalnum (*cp) || *cp == '_'))
	return 0;
    }
  return (cp != str);		//no empty strings, either
}
