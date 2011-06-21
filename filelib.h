#ifndef FILELIB_GUARD_H
#define FILELIB_GUARD_H

int retry_open(const char *file_name, int flags);
int retry_close(int fd);
int check_file(const char *file_name);
char *read_line(int fd);

#endif
