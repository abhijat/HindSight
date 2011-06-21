#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "filelib.h"
#include "strlib.h"

#define LINE_CHUNK 256

int
retry_open(const char *file_name, int flags)
{
    int fd;
    while ((fd = open(file_name, flags)), fd == -1 && errno == EINTR)
        ;
    if (fd == -1)
        perror("File open failed");
    return fd;
}

int
retry_close(int fd)
{
    int ret_val;
    while ((ret_val = close(fd)),ret_val == -1 && errno == EINTR)
        ;
    if (ret_val == -1)
        perror("File close failed");
    return ret_val;
}

int
check_file(const char *file_name)
{
    struct stat *statp;
    int ret_val;

    statp = malloc(sizeof(struct stat));
    ret_val = stat(file_name, statp);
    cleanup((void **)&statp);
    
    return ret_val;
}

char *
readline(int fd)
{
    char *buf;
    char c;
    int i, chunk, ret_val;

    buf = malloc(sizeof(char) * LINE_CHUNK);
    if (buf == NULL)
        return NULL;
    
    i = 0;
    chunk = 0;
    while ((ret_val = read(fd, &c, 1)) && c != '\n') {

        if (ret_val == -1 && errno == EINTR)
            continue;

        if (ret_val == -1) {
            perror("Failed to read string");
            free(buf);
            buf = NULL;
            return NULL;
        }
            
        buf[i++] = c;
        chunk++;

        if (chunk == LINE_CHUNK) {
            buf = realloc(buf, (i+LINE_CHUNK) * sizeof(char) );
            chunk = 0;
        }

    }

    if (c == '\n' || c == EOF)
        buf[i] = 0;
    if (i == 0)
        return NULL;
    buf = realloc(buf, (i+1) * sizeof(char));
    return buf;
}

int
main(int argc, char **argv)
{
    char *f;
    /*char *line;
      int fd;*/

    f = argv[1];
    if (check_file(f) != 0)
        printf("failed");
    else
        printf("success");
    printf("\n");
    /*
   fd = retry_open(f, O_RDONLY);
    while ((line = readline(fd)) != NULL)
        printf("%s\n", line);
    free(line);
    line = NULL;
    retry_close(fd);
    */
    return 0;
}
