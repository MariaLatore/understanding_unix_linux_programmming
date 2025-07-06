/* splitline.c - command reading and parsing functions for smsh
 *
 * char *next_cmd(char *prompt, FILE *fp) - get next command
 * char **splitline(char *str); - parse a string
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smsh.h"

char *
replace_str (const char *str, const char *old, const char *new)
{
  char *result;
  int i, cnt = 0;
  int newlen = strlen (new);
  int oldlen = strlen (old);

  // 计算old在str中出现的次数
  for (i = 0; str[i] != '\0'; i++)
    {
      if (strstr (&str[i], old) == &str[i])
	{
	  cnt++;
	  i += oldlen - 1;
	}
    }

  // 为新字符串分配内存
  result = (char *) emalloc (i + cnt * (newlen - oldlen) + 1);
  if (result == NULL)
    exit (EXIT_FAILURE);

  i = 0;
  while (*str)
    {
      // 检查当前位置是否匹配old
      if (strstr (str, old) == str)
	{
	  //  复制new到结果中
	  strcpy (&result[i], new);
	  i += newlen;
	  str += oldlen;
	}
      else
	{
	  //  复制原字符串的一个字符
	  result[i++] = *str++;
	}
    }
  result[i] = '\0';
  return result;
}

char *
next_cmd (char *prompt, FILE * fp)
/*
 * purpose: read next command line from fp
 * returns: dynamically allocated string holding command line
 * errors: NULL at EOF (not really an error)
 *         calls fatal from emalloc()
 * notes: allocates space in BUFSIZ chunks.
 */
{
  char *buf;			//the bffer
  int bufspace = 0;		//total size
  int pos = 0;			//current position
  int c;			//input char
  static int need_prompt = 1;
  int quote = 0;

  if (1 == need_prompt && NULL != prompt)
    {
      printf ("%s", prompt);	//prompt user
      fflush (stdout);
    }
  while ((c = getc (fp)) != EOF)
    {
/*need space?*/
      if (pos + 1 >= bufspace)
	{			//1 for \0
	  if (bufspace == 0)	//y: 1st time
	    buf = emalloc (BUFSIZ);
	  else			//or expand
	    buf = erealloc (buf, bufspace + BUFSIZ);
	  bufspace += BUFSIZ;
	}


      if (c == '"')
	{
	  quote = !quote;
	  buf[pos++] = c;
	  continue;
	}

      if (quote == 1)
	{
	  buf[pos++] = c;
	  continue;
	}

      if (c == ';')
	{
	  need_prompt = 0;
	  break;
	}
/*end of command?*/
      else if (c == '\n')
	{
	  need_prompt = 1;
	  break;
	}
      else if (c == ' ' && pos == 0)	//trim begin spaces
	continue;
      else
/*no, add to buffer */
	buf[pos++] = c;

      if (pos == 2 && strncmp (buf, "if", 2) == 0)
	break;
      if (pos == 5 && strncmp (buf, "while", 5) == 0)
	break;
    }
  if (c == EOF && pos == 0)	//EOF and no input
    return NULL;		//say so
  buf[pos] = '\0';

  //replace $$ to the shell process ID and $? to the result of the last command
  char processid[64];
  char lastcmdresult[64];
  snprintf (processid, 64, "%d", shell_process_id);
  snprintf (lastcmdresult, 64, "%d", last_cmd_result);
  char *buf1 = replace_str (buf, "$?", lastcmdresult);
  char *buf2 = replace_str (buf1, "$$", processid);
  free (buf);
  free (buf1);
  return buf2;
}

/* 
 * splitline ( parse a line into an array of strings )
 */
#define is_delim(x) ((x) == ' '||(x) == '\t')

char **
splitline (char *line)
/*
 * purpose: split a line into array of white-space separated tokens
 * returns: a NULL-terminated array of pointers to copies of the tokens
 *          or NULL if line if no tokens on the line
 * action: traverse the array, locate strings, make copies
 * note: strtok() could work, but we myay want to add quotes later
 */
{
  char **args;
  int spots = 0;		//spots in table
  int bufspace = 0;		//bytes in table
  int argnum = 0;		//slots used
  char *cp = line;		//pos in string
  char *start;
  int len;

  if (line == NULL)
    return NULL;

  args = emalloc (BUFSIZ);	//initialize array
  bufspace = BUFSIZ;
  spots = BUFSIZ / sizeof (char *);

  while (*cp != '\0')
    {
      while (is_delim (*cp))	//skip leading spaces
	cp++;
      if (*cp == '\0')		//quit at end-of-string
	break;

/*make sure the array has room (+1 for NULL)*/
      if (argnum + 1 >= spots)
	{
	  args = erealloc (args, bufspace + BUFSIZ);
	  bufspace += BUFSIZ;
	  spots += (BUFSIZ / sizeof (char *));
	}

/*mark start, then find end of word*/
      start = cp;
      len = 0;
      int inquote = 0;
      while ((*cp != '\0' && !(is_delim (*cp))) || 1 == inquote)
	{
	  if (*cp == '"')
	    inquote = !inquote;
	  len++;
	  cp++;
	}
      args[argnum++] = newstr (start, len);
    }
  args[argnum] = NULL;
  return args;
}

/*
 * purpose: constructor for strings
 * returns: a string, never NULL
 */
char *
newstr (char *s, int l)
{
  char *rv = emalloc (l + 1);
  rv[l] = '\0';
  strncpy (rv, s, l);
  return rv;
}

void
freelist (char **list)
/*
 * purpose: free the list returned by splitline
 * returns: nothing
 * action: free all strings in list and then free the list
 */
{
  char **cp = list;
  while (*cp)
    free (*cp++);
  free (list);
}

void *
emalloc (size_t n)
{
  void *rv;
  if ((rv = malloc (n)) == NULL)
    fatal ("out of memory", "", 1);
  return rv;
}

void *
erealloc (void *p, size_t n)
{
  void *rv;
  if ((rv = realloc (p, n)) == NULL)
    fatal ("realloc() failed", "", 1);
  return rv;
}
