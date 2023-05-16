#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <argp.h>

// This program is are recreation of touch. a linux core util that updates the edit time of a given file
// usage ./a.out [OPTION]... FILE...

const char *argp_program_version = "sleep 1.0";
const char *argp_program_bug_address = "<bug-gnu-utils@gnu.org>";
static char doc[] = "This program sleeps for a x amount of seconds";
static char args_doc[] = "[seconds]...";

// defining options
static struct argp_option options[]= {
    {"seconds", 's', 0, 0, "Given time is in seconds"},
    {"minutes", 'm', 0, 0, "Given time is in minutes"},
    {"hours", 'h', 0, 0, "Given time is in hours"},
    {"days", 'd', 0, 0, "Given time is in days"},
    {0}
};

struct arguments {
    double multiplier;
    double time;
};

/* Parse a single option. */
static error_t parse_opt (int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we
        know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;
    char *ptr;

    switch (key) {
        case 's':
            break;
        case 'm':
            arguments->multiplier = 60;
            break;
        case 'h':
            arguments->multiplier = 3600;
            break;
        case 'd':
            arguments->multiplier = 86400;
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num >= 1) {
                printf("Too many arguments passed by the user\n");
                argp_usage (state);
            }
            arguments->time = strtod(arg, &ptr);

            if(*ptr != 0) {
                printf("time given is invalid\n");
                exit(EXIT_FAILURE);
            }
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 1) {
                printf("No arguments passed by the user\n");
                argp_usage (state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };

int main(int argc, char **argv) {
    struct arguments arguments;
    /* Default values. */
    arguments.time = 0;
    arguments.multiplier = 1;

    argp_parse (&argp, argc, argv, 0, 0, &arguments);

    if(sleep(arguments.time * arguments.multiplier)== -1) {
        perror("sleep");
        printf("Errno: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    return(0);
}
