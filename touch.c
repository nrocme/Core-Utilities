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
#include <math.h>

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
    {"date", 'd', "STRING", 0, "Updates access time to a given date, example '2001-11-12 18:31:01'"},
    {"no-dereference", 'h', 0, 0, "Change only symbolic links"},
    {"file", 'f', 0, 0, "(ignored)"},
    {"modification", 'm', 0, 0, "Change modification time only"},
    {"reference", 'r', "FILE", 0, "Updates the file to the given files access and modification times"},
    {0}
};

struct arguments {
    int access, modification, create, usedate, nodereference, usereferencefile, help;
    char *filename, *datestring, *referencefilename;
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
        case 'r':
            arguments->usereferencefile = 1;
            arguments->referencefilename = arg;
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
    struct stat buf;
    int fd, rfd;
    char *arg;
    struct timespec times[2];
    /* Default values. */
    arguments.access = 1;
    arguments.modification = 1;
    arguments.create = 1;
    arguments.usedate = 0;
    arguments.datestring = NULL;
    arguments.nodereference = 0;
    arguments.usereferencefile = 0;
    arguments.referencefilename = NULL;
    arguments.filename = NULL;
    times[0].tv_nsec = 0;
    times[1].tv_nsec = 0;
    times[0].tv_sec = 0;
    times[1].tv_sec = 0;
    /* Parse our arguments; every option seen by parse_opt will
        be reflected in arguments. */
    argp_parse (&argp, argc, argv, 0, 0, &arguments);

    // Checking user options to decide how to update the files access and modification times.
    if(arguments.usedate){
        struct tm date;
        if(strptime(arguments.datestring, "%Y-%m-%d %H:%M:%S", &date) != NULL){
            time_t epoch = mktime(&date);
            times[0].tv_sec = epoch;
            times[1].tv_sec = epoch;
        } else {
            printf("Invalid time stamp given not file times were changed and no files were created\n");
            return(1);
        }
    } else if(arguments.usereferencefile){
        // open reference file descriptor
        rfd = openat(AT_FDCWD, arguments.referencefilename, O_WRONLY|O_LARGEFILE|O_NONBLOCK, 0666);
        // grab atime and mtime from fstat
        fstat(rfd, &buf);
        printf("%d\n", buf.st_atime);
        times[0].tv_sec = buf.st_atime;
        times[1].tv_sec = buf.st_mtime;
    } else {
        times[0].tv_nsec = UTIME_NOW;
        times[1].tv_nsec = UTIME_NOW;
    }

    // Opening file, to be changed, passed by the user. Checking the user flag for no-create. Creating the file if it does no exist and the user does not specify no-create option.
    if(arguments.create) fd = openat(AT_FDCWD, arguments.filename, O_WRONLY|O_CREAT|O_LARGEFILE|O_NONBLOCK, 0666);
    else fd = openat(AT_FDCWD, arguments.filename, O_WRONLY|O_LARGEFILE|O_NONBLOCK, 0666);


    // Checking access and modification flags set by user options.
    // UTIME_OMIT represents a special value that tells the system call futimes to not change the time that is currently set.
    if(!arguments.access) times[0].tv_nsec = UTIME_OMIT;
    if(!arguments.modification) times[1].tv_nsec = UTIME_OMIT;


    if(futimens(fd, times)== -1) {
        perror("futimens");
        printf("Errno: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    return(0);
}
