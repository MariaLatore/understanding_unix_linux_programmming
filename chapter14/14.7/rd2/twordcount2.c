/* twordcount2.c - threaded word counter for two files. */
/*                 version 2: uses mutex to lock counter */

#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_FILE_NUM 10

int total_words; // the counter and its lock
pthread_mutex_t counter_lock = PTHREAD_MUTEX_INITIALIZER;

int main(int ac, char *av[]) {
  pthread_t t[MAX_FILE_NUM]; // tow threads
  int i;
  int file_num;
  char **file_names;
  void *count_words(void *);

  if (ac <= 1) {
    printf("usage: %s file1 file2 ...\n", av[0]);
    exit(1);
  }
  file_num = ac - 1;
  file_names = &av[1];
  total_words = 0;
  for (i = 0; i < file_num; i++)
    pthread_create(&t[i], NULL, count_words, (void *)file_names[i]);

  for (i = 0; i < file_num; i++)
    pthread_join(t[i], NULL);

  printf("%5d: total words\n", total_words);
  return 0;
}

void *count_words(void *f) {
  char *filename = (char *)f;
  FILE *fp;
  int c, prevc = '\0';
  if ((fp = fopen(filename, "r")) != NULL) {
    while ((c = getc(fp)) != EOF) {
      if (!isalnum(c) && isalnum(prevc)) {
        pthread_mutex_lock(&counter_lock);
        total_words++;
        pthread_mutex_unlock(&counter_lock);
      }
      prevc = c;
    }
    fclose(fp);
  } else
    perror(filename);
  return NULL;
}
