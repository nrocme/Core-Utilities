#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>

long int findSize(char fileName[]);

// program documentating
const char *argp_program_version = "cp 1.0";
const char *argp_program_bug_address = "<bug-gnu-utils@gnu.org>";
static char doc[] = "This program copes contents SOURCE to DEST";
static char args_doc[] = "SOURCE DEST";

// defining options
static struct argp_option options[]= {
    {"interactive", 'i', 0, 0, "prompt before overwriting data"},
    {"no-clobber", 'n', 0, 0, "do not overwrite an existing file (overrides a previous -i option)"},
    {0}
};

struct arguments {
    int interactive, noclobber;
    char *pathSRC, *pathDEST;
};

/* Parse a single option. */
static error_t parse_opt (int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we
        know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;

    switch (key) {
        case 'i':
            arguments->interactive = 1;
            arguments->noclobber = 1;
            break;
        case 'n':
            arguments->noclobber = 1;
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num >= 2)
            /* Too many arguments. */
            argp_usage (state);
            if(state->arg_num == 0)arguments->pathSRC = arg;
            if(state->arg_num == 1)arguments->pathDEST = arg;
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 2)
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

long int findSize(char fileName[]) {
    // opening the file in read mode 
    FILE* fp = fopen(fileName, "r"); 
  
    // checks if the file exist returns -1 if not 
    if (fp == NULL) 
    {
        return -1; 
    } 
  
    fseek(fp, 0L, SEEK_END); 
  
    // ftell returns the position of the file pointer which is moved to the end of the file with fseek 
    long int size = ftell(fp); 
  
    // closing the file 
    fclose(fp); 
  
    return size; 
} 

int main(int argc, char *argv[]) {

    struct arguments arguments;
    char *arg;

    /* Default values. */
    arguments.interactive = 0;
    arguments.noclobber = 0;
    /* Parse our arguments; every option seen by parse_opt will
        be reflected in arguments. */
    argp_parse (&argp, argc, argv, 0, 0, &arguments);
    // gets path from the argument after ./a.out so to use cp you would need to ./a.out <pathname of copy> <where to put the copy>
    int bytesRead = 0;
    
    //call findsize to get the required size of buffer
    long int size = 0;
    size = findSize(arguments.pathSRC);
    void *buffer = malloc(size);
    //File descriptors for both the SRC and the DEST file DEST is created upon command use.
    int SRC = open(arguments.pathSRC, O_RDONLY);
    int DEST = open(arguments.pathDEST, O_WRONLY | O_CREAT , 00666);

    // hanlder interactive option
    if(arguments.noclobber && findSize(arguments.pathDEST) >= 1) {
        char input;
        if(arguments.interactive) {
            printf("DEST file is about to be overwritten.\nPlease type 'y' if so and 'n' to cancel...\n");
            scanf("%s", &input);
            if((int)input != 'y') {
                printf("DEST contains characters and will not be overwritten\n");
                exit(EXIT_FAILURE);
            }
        } else {
            printf("DEST contains characters and will not be overwritten\n");
            exit(EXIT_FAILURE);
        }
    }

    if (SRC == -1 || DEST == -1) {
        printf("Error Number %d/n", errno);
        perror("Program");
        return 0;
    }

    // read SRC store bytesRead holds how many bytes were read
    bytesRead = read(SRC, buffer, size);

    // Writes SRC from buffer to DEST
    write(DEST, buffer, bytesRead);
    printf("%s copied to %s successfully\n", arguments.pathSRC , arguments.pathDEST);
    return 0;
}
