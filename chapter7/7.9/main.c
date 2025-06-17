#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>

#define MAX_TEXT_LENGTH 10000

//Function to read the file content 
char *
read_file (const char *filename)
{
  FILE *file = fopen (filename, "r");
  if (file == NULL)
    {
      perror ("Error opening file");
      return NULL;
    }

  char *text = (char *) malloc (MAX_TEXT_LENGTH * sizeof (char));
  if (text == NULL)
    {
      perror ("Memory allocation failed");
      fclose (file);
      return NULL;
    }

  int i;
  for (i = 0; i < MAX_TEXT_LENGTH - 1; i++)
    {
      text[i] = fgetc (file);
      if (EOF == text[i])
	{
	  text[i] = '\0';
	  break;
	}
      else if ('\n' == text[i])
	text[i] = ' ';
    }
  text[MAX_TEXT_LENGTH - 1] = '\0';
  fclose (file);
  return text;
}

// Function to display the marquee
void
marquee_display (const char *text, int length, int position_y, float speed)
{
  initscr ();
  cbreak ();
  noecho ();

  if (position_y >= LINES)
    position_y = LINES - 1;


  int start = 0;
  int text_length = strlen (text);
  char visible_text[length + 1];
  int i;

  while (1)
    {
      clear ();
      for (i = 0; i < length; i++)
	{
	  visible_text[i] = text[(start + i) % text_length];
	}
      if (COLS > length)
	visible_text[length] = '\0';
      else
	visible_text[COLS] = '\0';


      mvprintw (position_y, 0, "%s", visible_text);
      move (0, 0);
      refresh ();

      start = (start + 1) % text_length;
      usleep ((1.0 / speed) * 1000000);
    }

  endwin ();
}

int
main (int argc, char *argv[])
{
  if (argc != 5)
    {
      fprintf (stderr, "Usage: %s <filename> <length> <position_y> <speed>\n",
	       argv[0]);
      return 1;
    }

  const char *filename = argv[1];
  int length = atoi (argv[2]);
  int position_y = atoi (argv[3]);
  float speed = atof (argv[4]);

  if (length > MAX_TEXT_LENGTH)
    length = MAX_TEXT_LENGTH;


  char *text = read_file (filename);
  if (text == NULL)
    {
      return 1;
    }

  marquee_display (text, length, position_y, speed);

  free (text);
  return 0;
}
