#include <main.h>

void help();


int main(int argc, char *argv[]){
    // OPTIONS
    bool opt_v = false;
    char fp[256][256];
    int i, nelems = 0;
    for(i=1; i<argc; i++) {
        if (argv[i][0] == '-' && argv[i][1]) {
                switch(argv[i][1]) {
                    case 'v':
                        opt_v = true;
                        break;
                    case '-':
                        // print help and exit program
                        // print help and exit program
                        if (strncmp("--help", argv[i], 6) == 0) {
                            help();
                            exit(EXIT_SUCCESS);
                        }
                        else if (strncmp("--verbose", argv[i], 6) == 0) opt_v = true;
                        else {
                            printf("%s argument is an invalid option\n", argv[i]);
                            printf("Please see 'mkdir --help\n");
                        }
                        break;
                    default:
                        // no options found executing command in default mode
                        printf("%s argument is an invalid option\n", argv[i]);
                        printf("Please see 'mkdir --help\n");
                        exit(EXIT_SUCCESS);
                    }
        }
        // its assumed that if there is a none option argument that it is a filepath target for ls relative to .
        else {
            strcpy(fp[nelems++], argv[i]);
        }
    }
    while(nelems > 0) {
        if(mkdir(fp[nelems-1], 0777) != 0) {
            perror("mkdir");
            exit(EXIT_SUCCESS);
        }
        nelems--;
        if(opt_v) printf("mkdir: created directory '%s'\n", fp[nelems+1]);
    }
    exit(EXIT_SUCCESS);
}

// displays help message
void help() {
    printf("Usage: mkdir [OPTION]... DIRECTORY...\n");
    printf("Create the DIRECTORY, if it does not already exist.\n");
    printf("OPTIONS\n");
    printf("    -v, --verbose   print a message for each created directory\n");
}
