/* webserv.c - a minimal web server (version 0.2)
 *     usage: ws portnumber
 *   feature: supports the GET command only
 *            runs in the current directory
 *            forks a new child to handle each reauest
 *            has MAJOR security holes, for demo purposes only
 *            has many other weaknesses, but is a good start
 *     build: gcc -Wall webserv.c -L. -lsock
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include "socklib.c"
#include "trim.c"

void read_til_crnl (FILE * fp);
void process_rq (char *rq, int fd);
void header (FILE * fp, char *content_type);
void cannot_do (int fd);
void do_404 (char *item, int fd);
int isadir (char *f);
int not_exist (char *f);
void do_ls (char *dir, int fd);
char *file_type (char *f);
int ends_in_cgi (char *f);
void do_cat (char *f, int fd);
void do_exec (char *prog, int fd);
void cat_header (FILE * fp, char *content_type, char *file_path);
void do_head (char *f, int fd);

int cgi_in_path (char *f);
int
main (int ac, char *av[])
{
  int sock, fd;
  FILE *fpin;
  char request[BUFSIZ];

  if (ac == 1)
    {
      fprintf (stderr, "usage: %s portnum\n", av[0]);
      exit (1);
    }

  sock = make_server_socket (atoi (av[1]));
  if (sock == -1)
    {
      puts ("make server socket error");
      exit (2);
    }

/* main loop here */
  while (1)
    {
/* take a call and buffer it */
      fd = accept (sock, NULL, NULL);
      if (fd == -1)
	perror ("accept error");
      fpin = fdopen (fd, "r");

//read request
      fgets (request, BUFSIZ, fpin);

      printf ("got a call: request = %s", request);
      read_til_crnl (fpin);

/* do what client asks */
      process_rq (request, fd);

      fclose (fpin);
    }
}

/* -------------------------------------------------
 * read_cli_crnl(FILE *)
 * skip over all request info until a CRNL is seen
 * ------------------------------------------------*/
void
read_til_crnl (FILE * fp)
{
  char buf[BUFSIZ];
  while (fgets (buf, BUFSIZ, fp) != NULL && strcmp (buf, "\r\n") != 0)
    ;
}

/* -----------------------------------------------
 * process_rq(char *rq, int fd)
 * do waht the request asks for and write reply to fd
 * handles request in a new process
 * rq is HTTP command: GET /foo/bar.html HTTP/1.0
 * -------------------------------------------------*/
void
process_rq (char *rq, int fd)
{
  char cmd[BUFSIZ], arg[BUFSIZ];

/* create a new process and return if not the child */
  if (fork () != 0)
    return;

  strcpy (arg, ".");		//precede args with ./
  if (sscanf (rq, "%s%s", cmd, arg + 1) != 2)
    return;

  if (strcmp (cmd, "HEAD") == 0)
    {
      puts ("do head");
      do_head (arg, fd);
    }
  else if (strcmp (cmd, "GET") != 0)
    {
      puts ("cannot_do");
      cannot_do (fd);
    }
  else if (not_exist (arg))
    {
      puts ("not exits");
      do_404 (arg, fd);
    }
  else if (isadir (arg))
    {
      puts ("isadira");
      do_ls (arg, fd);
    }
  else if (ends_in_cgi (arg) || cgi_in_path (arg))
    {
      puts ("ends in cgi or cgi-bin in path");
      setenv ("REQUEST_METHOD", "GET", 0);
      setenv ("SCRIPT_NAME", arg, 0);
      FILE *e = popen ("env", "r");
      char c;
      while ((c = fgetc (e)) != EOF)
	putchar (c);
      fclose (e);
      do_exec (arg, fd);
    }
  else
    {
      puts ("do_cat");
      do_cat (arg, fd);
    }
}

/*-----------------------------------------------------
 * the reply header thing: all functions need one
 * if content_type is NULL then don't send content type
 * ----------------------------------------------------*/
void
header (FILE * fp, char *content_type)
{
  time_t t = time (NULL);
  fprintf (fp, "HTTP/1.0  200 OK\r\n");
  if (content_type)
    fprintf (fp, "Content-type: %s\r\n", content_type);
  fprintf (fp, "Date: %s\r\n", ctime (&t));

}

/*-----------------------------------------------------
 * sinple function first:
 *     cannot_do(fd)   unimplemented HTTP command
 * and do_404*item, fd) no such object
 * ----------------------------------------------------*/
void
cannot_do (int fd)
{
  FILE *fp = fdopen (fd, "w");
  fprintf (fp, "HTTP/1.0 501 Not Implemented\r\n");
  fprintf (fp, "Content-type: text/plain\r\n");
  fprintf (fp, "\r\n");

  fprintf (fp, "That command is not yet implemented\r\n");
  fclose (fp);
}

void
do_404 (char *item, int fd)
{
  FILE *fp = fdopen (fd, "w");
  fprintf (fp, "HTTP/1.0 404 Not Found\r\n");
  fprintf (fp, "Content-type: text/plain\r\n");
  fprintf (fp, "\r\n");
  fprintf (fp, "The item you requested: %s\r\nis not found\r\n", item);
  fclose (fp);
}

/*------------------------------------------------
 * the directory listing section
 * isadir() uses stat, not_exist() uses stat
 * do_ls runs ls. It should not
 * -----------------------------------------------*/
int
isadir (char *f)
{
  struct stat info;
  return (stat (f, &info) != -1 && S_ISDIR (info.st_mode));
}

int
not_exist (char *f)
{
  struct stat info;
  return (stat (f, &info) == -1);
}

void
do_ls (char *dir, int fd)
{
  FILE *fp;

  fp = fdopen (fd, "w");
  header (fp, "text/plain");
  fprintf (fp, "\r\n");
  fflush (fp);

  dup2 (fd, 1);
  dup2 (fd, 2);
  close (fd);
  execlp ("ls", "ls", "-l", dir, NULL);
  perror (dir);
  exit (1);
}

/*------------------------------------------------------
 * the cgi stuff, function to check extention and
 * one to run the program
 *------------------------------------------------------*/
char *
file_type (char *f)
/* returns 'extension' of file */
{
  char *cp;
  if ((cp = strrchr (f, '.')) != NULL)
    return cp + 1;
  return "";
}

int
ends_in_cgi (char *f)
{
  return (strcmp (file_type (f), "cgi") == 0);
}

int
cgi_in_path (char *f)
{
  return (strstr (f, "/cgi-bin/") != NULL);
}

void
do_exec (char *prog, int fd)
{
  FILE *fp;

  fp = fdopen (fd, "w");
  header (fp, NULL);
  fflush (fp);
  dup2 (fd, 1);
  dup2 (fd, 2);
  close (fd);
  execl (prog, prog, NULL);
  perror (prog);
}

/*------------------------------------------------------
 * do_cat(filename, fd)
 * sends back contents after a header
 * -----------------------------------------------------*/
void
do_cat (char *f, int fd)
{
  char *extension = file_type (f);
  char *content = "text/plain";
  FILE *fpsock, *fpfile;
  int c;

  if (strcmp (extension, "html") == 0)
    content = "text/html";
  else if (strcmp (extension, "gif") == 0)
    content = "image/gif";
  else if (strcmp (extension, "jpg") == 0)
    content = "image/jpeg";
  else if (strcmp (extension, "jpeg") == 0)
    content = "image/jpeg";

  fpsock = fdopen (fd, "w");
  fpfile = fopen (f, "r");
  if (fpsock != NULL && fpfile != NULL)
    {
      cat_header (fpsock, content, f);
      fprintf (fpsock, "\r\n");
      while ((c = getc (fpfile)) != EOF)
	fputc (c, fpsock);
      puts ("transmit over!");
      fclose (fpfile);
      fclose (fpsock);
    }
  exit (0);
}

void
cat_header (FILE * fp, char *content_type, char *file_path)
{
  struct stat fileinfo;
  time_t t = time (NULL);
  fprintf (fp, "HTTP/1.0  200 OK\r\n");
  if (content_type)
    fprintf (fp, "Content-type: %s\r\n", content_type);
  fprintf (fp, "Date: %s\r\n", trim_string (ctime (&t)));
  if (0 == stat (file_path, &fileinfo))
    {
      fprintf (fp, "Last-modified: %s\r\n",
	       trim_string (ctime (&fileinfo.st_mtime)));
      fprintf (fp, "Content-length: %lu\r\n", fileinfo.st_size);
      fprintf (fp, "Accept-ranges: bytes\r\n");
      fprintf (fp, "Connection: close\r\n");
    }
}

void
do_head (char *f, int fd)
{
  char *extension = file_type (f);
  char *content = "text/plain";
  FILE *fpsock, *fpfile;

  if (strcmp (extension, "html") == 0)
    content = "text/html";
  else if (strcmp (extension, "gif") == 0)
    content = "image/gif";
  else if (strcmp (extension, "jpg") == 0)
    content = "image/jpeg";
  else if (strcmp (extension, "jpeg") == 0)
    content = "image/jpeg";

  fpsock = fdopen (fd, "w");
  fpfile = fopen (f, "r");
  if (fpsock != NULL && fpfile != NULL)
    {
      cat_header (fpsock, content, f);
      puts ("transmit over!");
      fclose (fpfile);
      fclose (fpsock);
    }
  exit (0);
}
