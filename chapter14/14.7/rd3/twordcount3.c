/* twordcount3.c - threaded word counter for 2 files
 *               - Version 3: one counter per file
 */
#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_FILE_NUM 10

struct arg_set { /* 2 values in one arg */
  char *fname;   /* fle to examine */
  int count;     /* number of words */
};

int main(int ac, char *av[]) {
  pthread_t t[MAX_FILE_NUM];
  struct arg_set args[MAX_FILE_NUM];
  int file_num;
  char **file_name;
  int i;
  int total_words = 0;
  void *count_words(void *);

  if (ac <= 1) {
    printf("usage: %s file1 file2 ...\n", av[0]);
    exit(1);
  }

  file_num = ac - 1;
  file_name = &av[1];

  for (i = 0; i < file_num; i++) {
    args[i].fname = file_name[i];
    args[i].count = 0;
  }

  for (i = 0; i < file_num; i++)
    pthread_create(&t[i], NULL, count_words, (void *)&args[i]);

  for (i = 0; i < file_num; i++)
    pthread_join(t[i], NULL);

  for (i = 0; i < file_num; i++) {
    printf("%5d: %s\n", args[i].count, file_name[i]);
    total_words += args[i].count;
  }

  printf("%5d: total words\n", total_words);
  return 0;
}

void *count_words(void *a) {

  struct arg_set *args = a; // cast arg back to correct type
  FILE *fp;
  int c, prevc = '\0';

  if ((fp = fopen(args->fname, "r")) != NULL) {
    while ((c = getc(fp)) != EOF) {
      if (!isalnum(c) && isalnum(prevc))
        args->count++;
      prevc = c;
    }
    fclose(fp);
  } else
    perror(args->fname);
  return NULL;
}
