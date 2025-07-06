#include <stdio.h>
#include<string.h>

typedef struct
{
  char name[BUFSIZ];
  char homedir[BUFSIZ];
  char shell[BUFSIZ];
} user_t;

int
getuser (char *name, user_t * ru)
{
  char buf[BUFSIZ] = { 0 };
  char modbuf[BUFSIZ] = { 0 };
  FILE *fp = fopen ("/etc/passwd", "r");
  char *token;
  int i, j;

  if (NULL == ru)
    {
      puts ("user nullptr");
      return -1;
    }

  if (NULL == fp)
    {
      puts ("error open /etc/passwd");
      return -1;
    }

  for (; NULL != fgets (buf, BUFSIZ, fp);
       bzero (buf, BUFSIZ), bzero (modbuf, BUFSIZ))
    {

      for (i = 0, j = 0; buf[i + 1] != 0; i++)
	{
	  if (buf[i] == ':' && buf[i + 1] == ':')
	    {
	      modbuf[j++] = buf[i];
	      modbuf[j++] = '#';
	    }
	  else
	    modbuf[j++] = buf[i];
	}


      token = strtok (modbuf, ":");	//name
      if (NULL == token)
	continue;
      if (strcmp (name, token) != 0)
	continue;
      else
	strncpy (ru->name, name, BUFSIZ);


      strtok (NULL, ":");	//password
      strtok (NULL, ":");	//UID
      strtok (NULL, ":");	//GID
      strtok (NULL, ":");	//GECOS
      token = strtok (NULL, ":");	//directory
      if (NULL == token)
	continue;
      else
	strncpy (ru->homedir, token, BUFSIZ);
      token = strtok (NULL, ":");	//shell
      if (NULL == token)
	continue;
      else
	{
	  strncpy (ru->shell, token, BUFSIZ);
	  return 0;
	}

    }
  return -1;
}
