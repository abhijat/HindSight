#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "config.h"
#include "strlib.h"
#include "triggers.h"

#define MAXLINE 256

float
get_cpu_load_atomic(void)
{
    char buf[MAXLINE];
    char c;
    char **list;
    int i, statfd;
    unsigned long total_time, idle_time;
    float idle_pct;

    while ((statfd = open("/proc/stat", O_RDONLY)),statfd == -1 && errno == EINTR)
        ;
    if (statfd == -1) {
        perror("Failed to read /proc/stat");
        return -1;
    }

    i = 0;
    while ((read(statfd, &c, 1) != -1) && c != '\n')
        buf[i++] = c;
    buf[i] = 0;

    list = string_to_list(buf, " ");

    total_time = 0;
    idle_time = atol(list[4]);
    for (i = 1; list[i] != 0; i++)
        total_time += atol(list[i]);
    
    idle_pct = ((float)idle_time/(float)total_time) * 100;
    destroy_list((void **)list);

    return idle_pct;
}

float
get_cpu_load_cumulative(void)
{
    int i;
    float sys_load;

    for (i = 0; i < 5; i++) {
        sys_load += get_cpu_load_atomic();
        sleep(0.05);
    }
    
    sys_load /= 5.0;
    printf("System idle time over 10 seconds is %f\n", sys_load);
    return sys_load;
}

int
vmstat_logger(struct trigger *trigp)
{
    pid_t child;
    char *pid_str;
    char *pid_file, *log_file;
    struct stat *statp;
    int fd;
    
    pid_file = "vmstat.pid";
    log_file = "vmstat.log";

    if (trigp->active == 0) {
        statp = malloc(sizeof(struct stat *));
        if (stat(pid_file, statp) == -1)
            return 0;
        pid_str = malloc(32 * sizeof(char));
        fd = open(pid_file, O_RDONLY);
        read(fd, pid_str, 32);
        printf("Killing %s\n", pid_str);
        close(fd);
        unlink(pid_file);
        kill(strtol(pid_str, NULL, 10), 9);
        return 0;
    }
    
    if ((child = fork()) == 0) {
        fd = open(log_file, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
        dup2(fd, STDOUT_FILENO);
        execlp(trigp->command, trigp->command, trigp->command_args, NULL);
        perror("Execlp failed");
    } else {
        unlink(pid_file);
        fd = open(pid_file, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWGRP);
        pid_str = num_to_str((long)child);
        write(fd, pid_str, strlen(pid_str));
        close(fd);
    }

    return 0;
}

int
main(int argc, char **argv)
{
    struct trigger *trigp;
    float threshold;

    threshold = 90.00;
    trigp = malloc(sizeof(struct trigger));
    trigp->active = 0;
    trigp->command = strdup("vmstat");
    trigp->command_args = strdup("1");
    trigp->log_handler = vmstat_logger;

    if ( argc == 2 && strcmp(argv[1], "kill") == 0) {
        trigp->log_handler(trigp);
        return 0;
    }

    if (get_cpu_load_cumulative() > threshold) {
        if (trigp->active == 0) {
            trigp->active = 1;
            trigp->log_handler(trigp);
        }
    } else {
        if (trigp->active == 1) {
            trigp->active = 0;
            trigp->log_handler(trigp);
        }
    }

    return 0;
}
