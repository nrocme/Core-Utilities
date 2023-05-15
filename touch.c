#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <argp.h>

void printHelp();
// This program is are recreation of touch. a linux core util that updates the edit time of a given file
// usage ./a.out [OPTION]... FILE...

const char *argp_program_version = "myTouch 1.0";
const char *argp_program_bug_address = "<bug-gnu-utils@gnu.org>";
static char doc[] = "This program updates modification and acces times of a file. If the file does not exist it will be created.";
static char args_doc[] = "[FILENAME]...";

// defining options
static struct argp_option options[]= {
    {"access", 'a', 0, 0, "Change access time only"},
    {"no-create", 'c', 0, 0, "Does not create any files"},
    {"date", 'd', "STRING", 0, "Updates access time to a given date, see GNU general date syntax"},
    {"no-dereference", 'h', 0, 0, "Change only symbolic links"},
    {"file", 'f', 0, 0, "(ignored)"},
    {"modification", 'm', 0, 0, "Change modification time only"},
    {"reference", 'r', "FILE", 0, "Updates the file to the given files access and modification times"},
    {0, 't', "STAMP", 0, "use timestamp in the format of YYYYMMDDhhmm.ss instead of current time"},
    {0}
};

struct arguments {
    int access, modification, create, usedate, nodereference, reference, help, usetimestamp;
    char *filename, *datestring, *referencefilename, *timestamp;
};

/* Parse a single option. */
static error_t parse_opt (int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we
        know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;

    switch (key) {
        case 'a':
            arguments->modification = 0;
            break;
        case 'm':
            arguments->access= 0;
            break;
        case 'c':
            arguments->create = 0;
            break;
        case 'f':
            break;
        case 'd':
            arguments->usedate = 1;
            arguments->datestring = arg;
            break;
        case 'h':
            arguments->nodereference = 1;
            break;
        case 't':
            arguments->usetimestamp = 1;
            arguments->timestamp = arg;
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num >= 1)
            /* Too many arguments. */
            argp_usage (state);
            arguments->filename = arg;
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 1)
            /* Not enough arguments. */
            argp_usage (state);
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
    int fd;
    char *arg;
    struct timespec times[2];
    /* Default values. */
    arguments.access = 1;
    arguments.modification = 1;
    arguments.create = 1;
    arguments.usedate = 0;
    arguments.datestring = NULL;
    arguments.nodereference = 0;
    arguments.reference = 0;
    arguments.usetimestamp = 0;
    arguments.timestamp = NULL;
    arguments.referencefilename = NULL;
    arguments.filename = NULL;
    times[0].tv_nsec = UTIME_NOW;
    times[1].tv_nsec = UTIME_NOW;
    /* Parse our arguments; every option seen by parse_opt will
        be reflected in arguments. */
    argp_parse (&argp, argc, argv, 0, 0, &arguments);

    if(arguments.create) fd = openat(AT_FDCWD, arguments.filename, O_WRONLY|O_CREAT|O_LARGEFILE|O_NONBLOCK, 0666);
    else fd = openat(AT_FDCWD, arguments.filename, O_WRONLY|O_LARGEFILE|O_NONBLOCK, 0666);


    if(!arguments.access) times[0].tv_nsec = UTIME_OMIT;
    if(!arguments.modification) times[1].tv_nsec = UTIME_OMIT;



    if(futimens(fd, times)== -1) {
        perror("futimens");
        printf("Errno: %d\n", errno);
        exit(EXIT_FAILURE);
    }

/*
    //Init variables
    int fd;
    char *option, *filepath;
    time_t currtime;
    struct tm *timeinfo;

    // Getting current localtime
    currtime = time(0);
    timeinfo = localtime (&currtime);
    //printf("Current system time: %s", asctime(timeinfo));
    if(argc == 1)
    for(int i = 1; i < argc; i++) printf("Try 'touch --help' for more information\n");
    if(argc == 3){
        // do nothing
    } else if(argc == 2) {
        option = argv[1];
        //check to make sure not asking for help page and that usage is valid
        if(strcmp(option, "--help") == 0) {
            printHelp();
        }
        else if(argv[1][0] == '-') {
            printf("Invalid use of option or unknown option.\n");
            printHelp();
        }
        else {
            // standard touch usage.
            // update file access and modification time.
            // if file does exist create the file
            filepath = argv[1];
            fd = open(filepath, O_CREAT);
            if(futimes(fd, NULL)== -1) {
                perror("futimens");
                printf("Errno: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }

    } else {
        // incorrect usage
        printf("Try 'touch --help' for more information\n");
    }*/
    return(0);
}

void printHelp() {
    printf("Help Page for Touch\n");
}
