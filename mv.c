#include <main.h>
// displays help message
void help();

int main(int argc, char *argv[]) 
{ 
    char *DEST = "";
    char *SRC = "";
    int i, j;
    
    if (argc < 2) {
        printf("mv: missing file operand\n");
        printf("Try 'mv --help' for more information.\n");
        exit(EXIT_SUCCESS);
    }
    
    for(i=1; i<argc; i++) {
        if (argv[i][0] == '-' && argv[i][1]) {
                switch(argv[i][1]) {
                    case '-':
                        // print help and exit program
                        if (strncmp("--help", argv[i], 6) == 0) {
                            help();
                            exit(EXIT_SUCCESS);
                        }
                        break;
                    default:
                        // no options found executing command in default mode
                        printf("%s argument is an invalid option\n", argv[i]);
                        printf("Please see 'mv --help\n");
                        exit(1);
                }
        }
        else {
            SRC = argv[i];
            if(argc > i+1) {
                DEST = argv[i+1];
                break;
            }
            else {
                printf("NO DEST SPECFIED SEE 'mv --help'\n");
                exit(EXIT_SUCCESS); 
            }
        }
    }
    if(rename(SRC, DEST) == 0) {
        printf("Move Successful!\n");
    }
    else perror("rename");
    exit(EXIT_SUCCESS);
} 
    
    
void help() {
    printf("USAGE: mv SRC DEST\n");
}
