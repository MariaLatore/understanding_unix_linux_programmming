#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smsh.h"


static IfStructure *collect_commands ();

void
do_if ()
{
  IfStructure *cmdlist = collect_commands ();
  execute_if_cmdlist (cmdlist);
  free_if_structure (cmdlist);
}

IfStructure *
collect_commands ()
{
  IfStructure *new_if_cmds = (IfStructure *) emalloc (sizeof (IfStructure));
  if (!new_if_cmds)
    return NULL;
  int ifstack = 1;
  int i = 0;
  char *cmdline = NULL;
  int bufsz;

//get condition cmd region
  cmdline = next_valid_cmdline ();
  new_if_cmds->condition = splitline (cmdline);
  free (cmdline);

  while (1)
    {
      cmdline = next_valid_cmdline ();
      if (strcmp (cmdline, "then") != 0)
	{
	  free (cmdline);
	}
      else
	{
	  free (cmdline);
	  break;
	}
    }


//get then cmd region
  new_if_cmds->then_list = (char ***) emalloc (MBLOCK * sizeof (char **));
  if (!new_if_cmds->then_list)
    {
      free_if_structure (new_if_cmds);
      return NULL;
    }
  else
    bufsz = MBLOCK;

  while (1)
    {
      if (0 == ifstack)
	{
	  new_if_cmds->then_list[i++] = NULL;
	  break;
	}

      cmdline = next_valid_cmdline ();
      if (strcmp (cmdline, "else") == 0)
	{
	  if (ifstack == 1)
	    {
	      new_if_cmds->then_list[i++] = NULL;
	      free (cmdline);
	      break;
	    }
	  else
	    {
	      new_if_cmds->then_list[i++] = splitline (cmdline);
	      if (i == bufsz - 1)
		{
		  new_if_cmds->then_list =
		    erealloc (new_if_cmds->then_list, bufsz + MBLOCK);
		  bufsz += MBLOCK;
		}
	      free (cmdline);
	      continue;
	    }
	}
      else if (strcmp (cmdline, "if") == 0)
	{
	  ifstack++;
	  new_if_cmds->then_list[i++] = splitline (cmdline);
	  if (i == bufsz - 1)
	    {
	      new_if_cmds->then_list =
		erealloc (new_if_cmds->then_list, bufsz + MBLOCK);
	      bufsz += MBLOCK;
	    }
	  free (cmdline);
	  continue;
	}
      else if (strcmp (cmdline, "fi") == 0)
	{
	  if (ifstack-- > 1)
	    {
	      new_if_cmds->then_list[i++] = splitline (cmdline);
	      if (i == bufsz - 1)
		{
		  new_if_cmds->then_list =
		    erealloc (new_if_cmds->then_list, bufsz + MBLOCK);
		  bufsz += MBLOCK;
		}
	    }
	  free (cmdline);
	  continue;
	}
      else
	{
	  new_if_cmds->then_list[i++] = splitline (cmdline);
	  if (i == bufsz - 1)
	    {
	      new_if_cmds->then_list =
		erealloc (new_if_cmds->then_list, bufsz + MBLOCK);
	      bufsz += MBLOCK;
	    }
	  free (cmdline);
	}
    }


  if (0 == ifstack)
    {
      new_if_cmds->else_list = (char ***) emalloc (sizeof (char **));
      new_if_cmds->else_list[0] = NULL;
      return new_if_cmds;
    }

  //get else cmd region
  new_if_cmds->else_list = (char ***) emalloc (sizeof (char **) * MBLOCK);
  if (!new_if_cmds->else_list)
    {
      free_if_structure (new_if_cmds);
      return NULL;
    }

  i = 0;
  bufsz = MBLOCK;

  while (1)
    {
      if (0 == ifstack)
	{
	  new_if_cmds->else_list[i++] = NULL;
	  break;
	}

      cmdline = next_valid_cmdline ();
      if (strcmp (cmdline, "if") == 0)
	{
	  ifstack++;
	  new_if_cmds->else_list[i++] = splitline (cmdline);
	  if (i == bufsz - 1)
	    {
	      new_if_cmds->else_list =
		erealloc (new_if_cmds->else_list, bufsz + MBLOCK);
	      bufsz += MBLOCK;
	    }
	  free (cmdline);
	  continue;
	}
      else if (strcmp (cmdline, "fi") == 0)
	{
	  ifstack--;
	  if (ifstack > 0)
	    {
	      new_if_cmds->else_list[i++] = splitline (cmdline);
	      if (i == bufsz - 1)
		{
		  new_if_cmds->else_list =
		    erealloc (new_if_cmds->else_list, bufsz + MBLOCK);
		  bufsz += MBLOCK;
		}
	    }
	  free (cmdline);
	  continue;
	}
      else
	{
	  new_if_cmds->else_list[i++] = splitline (cmdline);
	  if (i == bufsz - 1)
	    {
	      new_if_cmds->else_list =
		erealloc (new_if_cmds->else_list, bufsz + MBLOCK);
	      bufsz += MBLOCK;
	    }
	  free (cmdline);
	}
    }
  return new_if_cmds;
}

void
free_if_structure (IfStructure * if_struct)
{
  int i;
  freelist (if_struct->condition);
  if (if_struct->then_list != NULL)
    {
      for (i = 0; if_struct->then_list[i] != NULL; i++)
	{
	  freelist (if_struct->then_list[i]);
	}
      free (if_struct->then_list);
    }
  if (if_struct->else_list != NULL)
    {
      for (i = 0; if_struct->else_list[i] != NULL; i++)
	{
	  freelist (if_struct->else_list[i]);
	}
      free (if_struct->else_list);
    }
  free (if_struct);
}



void
execute_if_cmdlist (IfStructure * cmdlist)
{
  int i;
  int result;
  IfStructure *nest_cmdlist;
  WhileStruct *nest_while_cmdlist;
  int nest_cmdlist_len;
  result = execute (cmdlist->condition);
  if (result == 0)
    for (i = 0; cmdlist->then_list[i] != NULL; i++)
      {
	if (strcmp (cmdlist->then_list[i][0], "if") == 0)
	  {
	    if ((nest_cmdlist_len =
		 generate_if_cmdlist (&cmdlist->then_list[++i],
				      &nest_cmdlist)) > 0)
	      {
		i = i + nest_cmdlist_len - 1;
		execute_if_cmdlist (nest_cmdlist);
		free_if_structure (nest_cmdlist);
	      }
	  }
	else if (strcmp (cmdlist->then_list[i][0], "while") == 0)
	  {
	    if ((nest_cmdlist_len =
		 generate_while_cmdlist (&cmdlist->then_list[++i],
					 &nest_while_cmdlist)) > 0)
	      {
		i = i + nest_cmdlist_len - 1;
		execute_while_cmdlist (nest_while_cmdlist);
		free_while_structure (nest_while_cmdlist);
	      }
	  }

	else
	  execute (cmdlist->then_list[i]);
      }
  else
    for (i = 0; cmdlist->else_list[i] != NULL; i++)
      {
	if (strcmp (cmdlist->else_list[i][0], "if") == 0)
	  {
	    if ((nest_cmdlist_len =
		 generate_if_cmdlist (&cmdlist->else_list[++i],
				      &nest_cmdlist)) > 0)
	      {
		i = i + nest_cmdlist_len - 1;
		execute_if_cmdlist (nest_cmdlist);
		free_if_structure (nest_cmdlist);
	      }
	  }
	else if (strcmp (cmdlist->else_list[i][0], "while") == 0)
	  {
	    if ((nest_cmdlist_len =
		 generate_while_cmdlist (&cmdlist->else_list[++i],
					 &nest_while_cmdlist)) > 0)
	      {
		i = i + nest_cmdlist_len - 1;
		execute_while_cmdlist (nest_while_cmdlist);
		free_while_structure (nest_while_cmdlist);
	      }
	  }
	else
	  execute (cmdlist->else_list[i]);
      }

}


int
generate_if_cmdlist (char ***arglist, IfStructure ** ret_cmdlist)
{
  int i;
  int condition_region_len = 1;	//condition is only one line command
  int then_region_begin = 0;
  int then_region_end = 0;
  int then_region_len = 0;
  int else_region_begin = 0;
  int else_region_end = 0;
  int else_region_len = 0;
  int have_else_region = 0;
  int ifstack = 0;
  if (arglist == NULL || *arglist == NULL)
    {
      puts ("arglist NULL ptr");
      return -1;
    }

  (*ret_cmdlist) = (IfStructure *) malloc (sizeof (IfStructure));
  if (!(*ret_cmdlist))
    {
      puts ("ret_cmdlist malloc error");
      return -1;
    }

  //get condition cmd region
  (*ret_cmdlist)->condition = newstrlist (arglist[0]);
  if (arglist[1] == NULL || strcmp (arglist[1][0], "then") != 0)
    {
      free_if_structure (*ret_cmdlist);
      *ret_cmdlist = NULL;
      puts ("then lacked");
      return -1;
    }

  //get then cmd region
  then_region_begin = condition_region_len + 1;	//+1 for symbol 'then'
  for (i = then_region_begin, ifstack = 0; arglist[i] != NULL; i++)
    {
      if (strcmp (arglist[i][0], "else") == 0 && ifstack == 0)
	break;
      else if (strcmp (arglist[i][0], "fi") == 0)
	{
	  if (ifstack == 0)
	    break;
	  else
	    ifstack--;
	}
      else if (strcmp (arglist[i][0], "if") == 0)
	ifstack++;
    }
  if (arglist[i] == NULL)
    {
      free_if_structure (*ret_cmdlist);
      *ret_cmdlist = NULL;
      puts ("unexpected NULL");
      return -1;
    }
  else if (strcmp (arglist[i][0], "else") == 0)
    {
      if (arglist[i][1] != NULL)
	{
	  free_if_structure (*ret_cmdlist);
	  *ret_cmdlist = NULL;
	  puts ("else should occupy a single line.\n");
	  return -1;
	}
      have_else_region = 1;
    }
  else if (strcmp (arglist[i][0], "fi") == 0)
    {
      if (arglist[i][1] != NULL)
	{
	  free_if_structure (*ret_cmdlist);
	  *ret_cmdlist = NULL;
	  puts ("fi should occupy a single line");
	  return -1;
	}
      have_else_region = 0;
    }

  then_region_end = i;
  then_region_len = then_region_end - then_region_begin;
  (*ret_cmdlist)->then_list = (char ***) malloc (sizeof (char **) * (then_region_len + 1));	//extra +1 for ending NULL
  if (!(*ret_cmdlist)->then_list)
    {
      free_if_structure (*ret_cmdlist);
      *ret_cmdlist = NULL;
      puts ("malloc then_list error");
      return -1;
    }
  for (i = 0; i < then_region_len; i++)
    (*ret_cmdlist)->then_list[i] =
      newstrlist (arglist[i + then_region_begin]);
  (*ret_cmdlist)->then_list[i] = NULL;
  if (!have_else_region)
    {
      (*ret_cmdlist)->else_list = (char ***) malloc (sizeof (char **));
      (*ret_cmdlist)->else_list[0] = NULL;
      return condition_region_len + 1 /*'then' cmd */  +
	then_region_len + 1 /*'fi' cmd */ ;
    }


  //get else cmd region
  else_region_begin = then_region_end + 1;
  for (i = else_region_begin, ifstack = 0; arglist[i] != NULL; i++)
    {
      if (strcmp (arglist[i][0], "fi") == 0)
	{
	  if (ifstack == 0)
	    break;
	  else
	    ifstack--;
	}
      else if (strcmp (arglist[i][0], "if") == 0)
	ifstack++;
    }

  if (arglist[i] == NULL)
    {
      free_if_structure (*ret_cmdlist);
      *ret_cmdlist = NULL;
      puts ("else unexpected NULL arglist ptr");
      return -1;
    }
  else if (strcmp (arglist[i][0], "fi") == 0)
    {
      if (arglist[i][1] != NULL)
	{
	  free_if_structure (*ret_cmdlist);
	  *ret_cmdlist = NULL;
	  puts ("fi should ocupy a single line");
	  return -1;
	}
    }

  else_region_end = i;
  else_region_len = else_region_end - else_region_begin;
  (*ret_cmdlist)->else_list = (char ***) malloc (sizeof (char **) * (else_region_len + 1));	//+1 for ending NULL
  for (i = 0; i < else_region_len; i++)
    (*ret_cmdlist)->else_list[i] =
      newstrlist (arglist[i + else_region_begin]);
  (*ret_cmdlist)->else_list[i] = NULL;
  return condition_region_len + 1 /*'then' cmd */  +
    then_region_len + 1 /*'else' cmd */  + else_region_len +
    1 /*'fi' cmd */ ;
}


char **
newstrlist (char **list)
{

  int i, len;
  for (i = 0; list[i] != NULL; i++);
  len = i + 1;
  char **newlist = (char **) malloc (len * sizeof (char *));
  for (i = 0; list[i] != NULL; i++)
    newlist[i] = newstr (list[i], strlen (list[i]));
  newlist[i] = NULL;
  return newlist;
}
