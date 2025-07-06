#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smsh.h"

static WhileStruct *collect_while_commands ();

void
do_while ()
{
  WhileStruct *cmdlist = collect_while_commands ();
  execute_while_cmdlist (cmdlist);
  free_while_structure (cmdlist);
}

char *
next_valid_cmdline ()
{
  char *cmdline;
  char **splitcmd;
  while (1)
    {
      if ((cmdline = next_cmd (NULL, stdin)) == NULL)
	continue;
      splitcmd = splitline (cmdline);
      if (NULL == splitcmd)
	{
	  free (cmdline);
	  continue;
	}
      else if (NULL == splitcmd[0])	//when comline="\n", we'll go into this situation
	{
	  freelist (splitcmd);
	  free (cmdline);
	  continue;
	}
      break;
    }

  freelist (splitcmd);
  return cmdline;
}


WhileStruct *
collect_while_commands ()
{

  WhileStruct *new_while_cmds =
    (WhileStruct *) emalloc (sizeof (WhileStruct));
  if (!new_while_cmds)
    return NULL;
  int whilestack = 1;
  int i = 0;
  char *cmdline;
  int bufsz;

//get condition cmd region
  cmdline = next_valid_cmdline ();
  new_while_cmds->condition = splitline (cmdline);
  free (cmdline);

  while (1)
    {
      cmdline = next_valid_cmdline ();
      if (strcmp (cmdline, "do") != 0)
	{
	  free (cmdline);
	}
      else
	{
	  free (cmdline);
	  break;
	}
    }

//get loop block region
  new_while_cmds->loopblock = (char ***) emalloc (MBLOCK * sizeof (char **));
  if (!new_while_cmds->loopblock)
    {
      free_while_structure (new_while_cmds);
      return NULL;
    }
  else
    bufsz = MBLOCK;

  while (1)
    {
      if (0 == whilestack)
	{
	  new_while_cmds->loopblock[i++] = NULL;
	  break;
	}

      cmdline = next_valid_cmdline ();
      if (strcmp (cmdline, "while") == 0)
	{
	  whilestack++;
	  new_while_cmds->loopblock[i++] = splitline (cmdline);
	  if (i == bufsz - 1)
	    {
	      new_while_cmds->loopblock =
		erealloc (new_while_cmds->loopblock, bufsz + MBLOCK);
	      bufsz += MBLOCK;
	    }
	  free (cmdline);
	  continue;
	}
      else if (strcmp (cmdline, "done") == 0)
	{
	  if (whilestack-- > 1)
	    {
	      new_while_cmds->loopblock[i++] = splitline (cmdline);
	      if (i == bufsz - 1)
		{
		  new_while_cmds->loopblock =
		    erealloc (new_while_cmds->loopblock, bufsz + MBLOCK);
		  bufsz += MBLOCK;
		}
	      free (cmdline);
	      continue;
	    }
	}
      else
	{
	  new_while_cmds->loopblock[i++] = splitline (cmdline);
	  if (i == bufsz - 1)
	    {
	      new_while_cmds->loopblock =
		erealloc (new_while_cmds->loopblock, bufsz + MBLOCK);
	      bufsz += MBLOCK;
	    }
	  free (cmdline);
	}
    }
  return new_while_cmds;
}

void
free_while_structure (WhileStruct * while_struct)
{
  int i;
  freelist (while_struct->condition);
  if (while_struct->loopblock != NULL)
    {
      for (i = 0; while_struct->loopblock[i] != NULL; i++)
	freelist (while_struct->loopblock[i]);
    }

  free (while_struct);
}


void
execute_while_cmdlist (WhileStruct * cmdlist)
{
  int i;
  int result;
  WhileStruct *nest_cmdlist;
  IfStructure *nest_if_cmdlist;
  int nest_cmdlist_len;
  while (1)
    {
      result = execute (cmdlist->condition);
      if (result != 0)
	break;
      for (i = 0; cmdlist->loopblock[i] != NULL; i++)
	{
	  if (strcmp (cmdlist->loopblock[i][0], "while") == 0)
	    {
	      if ((nest_cmdlist_len =
		   generate_while_cmdlist (&cmdlist->loopblock[++i],
					   &nest_cmdlist)) > 0)
		{
		  i = i + nest_cmdlist_len - 1;
		  execute_while_cmdlist (nest_cmdlist);
		  free_while_structure (nest_cmdlist);
		}
	    }
	  else if (strcmp (cmdlist->loopblock[i][0], "if") == 0)
	    {
	      if ((nest_cmdlist_len =
		   generate_if_cmdlist (&cmdlist->loopblock[++i],
					&nest_if_cmdlist)) > 0)
		{
		  i = i + nest_cmdlist_len - 1;
		  execute_if_cmdlist (nest_if_cmdlist);
		  free_if_structure (nest_if_cmdlist);
		}
	    }
	  else
	    execute (cmdlist->loopblock[i]);

	}
    }
  return;
}


int
generate_while_cmdlist (char ***arglist, WhileStruct ** ret_cmdlist)
{
  int i;
  int condition_region_len = 1;	//condition is only one line command
  int loop_region_begin = 0;
  int loop_region_end = 0;
  int loop_region_len = 0;
  int whilestack = 0;

  if (arglist == NULL || *arglist == NULL)
    {
      puts ("arglist null ptr");
      return -1;
    }

  (*ret_cmdlist) = (WhileStruct *) malloc (sizeof (WhileStruct));
  if (!(*ret_cmdlist))
    {
      puts ("while ret_cmdlist malloc error");
      return -1;
    }

//get condition cmd region
  (*ret_cmdlist)->condition = newstrlist (arglist[0]);
  if (arglist[1] == NULL || strcmp (arglist[1][0], "do") != 0)
    {
      free_while_structure (*ret_cmdlist);
      *ret_cmdlist = NULL;
      puts ("do lacked");
      return -1;
    }

//get loop block
  loop_region_begin = condition_region_len + 1;	//+1 for symbol 'do'
  for (i = loop_region_begin, whilestack = 0; arglist[i] != NULL; i++)
    {
      if (strcmp (arglist[i][0], "done") == 0)
	{
	  if (whilestack == 0)
	    break;
	  else
	    whilestack--;
	}
      else if (strcmp (arglist[i][0], "while") == 0)
	whilestack++;

    }

  if (arglist[i] == NULL)
    {
      free_while_structure (*ret_cmdlist);
      *ret_cmdlist = NULL;
      puts ("unexpected NULL while");
      return -1;
    }
  else if (strcmp (arglist[i][0], "done") == 0)
    {
      if (arglist[i][1] != NULL)
	{
	  free_while_structure (*ret_cmdlist);
	  *ret_cmdlist = NULL;
	  puts ("done should occupy a single line");
	  return -1;
	}
    }
  else
    {
      puts ("while parse unexpected terminate");
      free_while_structure (*ret_cmdlist);
      *ret_cmdlist = NULL;
      return -1;
    }

  loop_region_end = i;
  loop_region_len = loop_region_end - loop_region_begin;
  (*ret_cmdlist)->loopblock = (char ***) malloc (sizeof (char **) * (loop_region_len + 1));	//extra +1 for ending NULL
  if (!(*ret_cmdlist)->loopblock)
    {
      free_while_structure (*ret_cmdlist);
      *ret_cmdlist = NULL;
      puts ("malloc then_list error");
      return -1;
    }
  for (i = 0; i < loop_region_len; i++)
    (*ret_cmdlist)->loopblock[i] =
      newstrlist (arglist[i + loop_region_begin]);
  (*ret_cmdlist)->loopblock[i] = NULL;

  return condition_region_len + 1 /*'do'cmd */  + loop_region_len +
    1 /*'done' cmd */ ;
}
