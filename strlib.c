#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_SIZE 8

char **
string_to_list(const char *str, const char *delim)
{
    char *tmp, *line, *context;
    char **list;
    int i, chunk_size;

    i = 0;
    chunk_size = 0;

    line = strdup(str);
    if (line == NULL)
        return NULL;

    if ((tmp = strtok_r(line, delim, &context)) != NULL) {
        list = malloc(CHUNK_SIZE * sizeof(char *));
        if (list == NULL) {
            free(line);
            line = NULL;
            return NULL;
        }
        list[i++] = strdup(tmp);
        chunk_size++;
    } else {
        free(line);
        line = NULL;
        return NULL;
    }

    while ((tmp = strtok_r(NULL, delim, &context)) != NULL) {
        if (chunk_size == CHUNK_SIZE) {
            list = realloc(list, (i+CHUNK_SIZE) * sizeof(char *));
            chunk_size = 0;
        }
        list[i++] = strdup(tmp);
        chunk_size++;
    }

    list[i] = (char *) 0;
    list = realloc(list, (i+1) * sizeof(char *));

    free(line);
    line = NULL;
    
    free(tmp);
    tmp = NULL;

#ifdef DEBUG
    printf("Allocated %d elements\n", i);
#endif
    return list;
}

void
destroy_list(void **list)
{
    int i;
    for (i = 0; list[i] != 0; i++) {
#ifdef DEBUG
        printf("Freeing %s\n", (char *)list[i]);
#endif
        free(list[i]);
        list[i] = NULL;
    }

#ifdef DEBUG
        printf("Freeing list\n");
#endif
    free(list);
    list = NULL;
    return;
}

char *
num_to_str(long num)
{
    char *str;
    int i, len;
    long rem;

    rem = 1;
    len = 0;
    i = 0;

    while ((num / (rem)) > 0) {
        len++;
        rem *= 10;
    }

    str = malloc(len * sizeof(char));
    rem /= 10;

    while (rem > 0) {
        str[i++] = (num/rem) + 48;
        num = num%rem;
        rem /= 10;
    }
    
    str[i] = 0;

    return str;
}

void
cleanup(void **ptr)
{
    free(*ptr);
    *ptr = NULL;
    return;
}

char *
lstrip(const char *str)
{
    int i;
    char *ret;

    i = 0;

    if (strlen(str) == 0)
        return NULL;
    
    while (isspace(str[i]))
           i++;
    ret = malloc((strlen(str)-i) * sizeof(char));
    if (ret == NULL)
        return NULL;
    strncpy(ret, str+i, strlen(str) - i);
    return ret;
}

char *
rstrip(const char *str)
{
    int i;
    char *ret;

    if (strlen(str) == 0)
        return NULL;

    i = strlen(str) - 1;
    while (isspace(str[i]))
        i++;
    ret = malloc((i+1) * sizeof(char));
    if (ret == NULL)
        return NULL;
    strncpy(ret, str, i);
    ret[i+1] = 0;
    return ret;
}

char *
strip(const char *str)
{
    char *ret;
    ret = lstrip(str);
    if (ret == NULL)
        return NULL;
    ret = rstrip(ret);
    return ret;
}

/*
int
main(void)
{
    long f;
    char *str;

    f = 1234567;
    str = "		   str		   ";
    printf("%s\n", strip(str));

    return 0;
}
*/
