/*utmplib.c - functions to buffer reads from utmp file
 *
 *
 *    functions are
 *        utmp_open(filename) - open file
 *            returns -1 on error
 *        utmp_next()         - return pointer ro next struct
 *            returns NULL on eof
 *        utmp_close()        - close file
 *        reads NRECS per read and then doles them out from the buffer
 */
#ifndef _UTMPLIB_H_
#define _UTMPLIB_H_

#define NRECS  16
#define NULLUT ((struct utmp *)NULL)
#define UTSIZE (sizeof(struct utmp))
typedef int seek_utmp_base_t;

int utmp_open(char *);
int utmp_reload();
void utmp_close();
int utmp_seek(int, seek_utmp_base_t);

#endif
