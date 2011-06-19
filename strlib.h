#ifndef GUARD_STRLIB_H
#define GUARD_STRLIB_H

char **string_to_list(const char *stri, const char *delim);
void destroy_list(void **list);
char *num_to_str(long num);

#endif
