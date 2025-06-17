/* execute.c - code used by small shell to execute commands */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

int shell_cd (char **);

int
execute (char *argv[])
/*
 * purpose: run a program passing it arguments
 * returns: status returned via wait, or -1 on error
 * errors: -1 on fork() or wait() errors
 */
{
  int pid;
  int child_info = -1;

  if (argv[0] == NULL)		//nothing succeeds
    return 0;

  if (strcmp (argv[0], "cd") == 0)
    return shell_cd (argv);
  if ((pid = fork ()) == -1)
    perror ("fork");
  else if (pid == 0)
    {
      signal (SIGINT, SIG_DFL);
      signal (SIGQUIT, SIG_DFL);
      execvp (argv[0], argv);
      perror ("cannot execute command");
      printf ("cmd:%s\n", argv[0]);
      exit (1);
    }
  else
    {
      if (wait (&child_info) == -1)
	perror ("wait");
    }
  return child_info;
}


/* 内置cd命令的实现 */
int
shell_cd (char **args)
{
  char *dir;
  char cwd[1024];		// 存储当前工作目录

  // 如果没有参数，默认切换到HOME目录
  if (args[1] == NULL)
    {
      dir = getenv ("HOME");
      if (dir == NULL)
	{
	  fprintf (stderr, "cd: HOME环境变量未设置\n");
	  return 1;
	}
    }
  //                                                             // 如果参数是"-"，切换到OLDPWD目录
  else if (strcmp (args[1], "-") == 0)
    {
      dir = getenv ("OLDPWD");
      if (dir == NULL)
	{
	  fprintf (stderr, "cd: OLDPWD环境变量未设置\n");
	  return 1;
	}
      printf ("%s\n", dir);	// 打印切换到的目录
    }
  // 否则切换到指定的目录
  else
    {
      dir = args[1];
    }

  // 保存当前工作目录到OLDPWD
  if (getcwd (cwd, sizeof (cwd)) == NULL)
    {
      perror ("cd: 获取当前目录失败");
      return 1;
    }
  setenv ("OLDPWD", cwd, 1);

  // 执行chdir系统调用切换目录
  if (chdir (dir) != 0)
    {
      perror ("cd");
      return 1;
    }

  // 更新PWD环境变量
  if (getcwd (cwd, sizeof (cwd)) == NULL)
    {
      perror ("cd: 获取新目录失败");
      return 1;
    }
  setenv ("PWD", cwd, 1);

  return 0;
}
