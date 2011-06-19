#ifndef GUARD_CONFIG_HEADER
#define GUARD_CONFIG_HEADER

struct trigger {
    int active;
    char *command;
    char *command_args; /* End this with a 0 */
    int (*check_trigger)(struct trigger *);
    int (*log_handler)(struct trigger *);
    struct trigger *next;
};

#endif
