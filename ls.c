#include <main.h>

// displays help message
void help();
void parsemode(int modemask, char *mode);
struct stat getstats(char *name, char *fp);
struct dirent **bubbleSort(int nElems, struct dirent **namelist, char *fp);
struct dirent **swap(int i, int j, struct dirent **namelist);

int main(int argc, char *argv[]) 
{ 
    // OPTIONS FLAGS
    bool 
        opt_a = false, 
        opt_l = false, 
        opt_A = false, 
        opt_U = false,
        opt_S = false,
        opt_GID = true,
        opt_AUTHOR = false;
    char *fp= ".";
    int i, n;
    struct dirent **namelist;
    struct stat stats;
    
    for(i=1; i<argc; i++) {
        if (argv[i][0] == '-' && argv[i][1]) {
                switch(argv[i][1]) {
                    case 'a':
                        opt_a = true;
                        break;
                    case 'h':
                        help();
                        exit(EXIT_SUCCESS);
                        break;
                    case 'l':
                        opt_l = true;
                        break;
                    case 'A':
                        opt_A = true;
                        break;
                    case 'U':
                        opt_U = true;
                        break;
                    case 'G':
                        opt_GID = false;
                        break;
                    case 'S':
                        opt_S = true;
                        break;
                    case '-':
                        // print help and exit program
                        if (strcmp("--help", argv[i]) == 0) {
                            help();
                            exit(EXIT_SUCCESS);
                        }
                        else if (strcmp("--all", argv[i]) == 0) opt_a = true;
                        else if (strcmp("--long", argv[i]) == 0) opt_l = true;
                        else if (strcmp("--almost-all", argv[i]) == 0) opt_A = true;
                        else if (strcmp("--author", argv[i]) == 0) opt_AUTHOR = true;
                        else {
                            printf("%s argument is an invalid option\n", argv[i]);
                            printf("Please see 'ls --help\n");
                            exit(EXIT_SUCCESS);
                        }
                        break;
                    default:
                        // no options found executing command in default mode
                        printf("%s argument is an invalid option\n", argv[i]);
                        printf("Please see 'ls --help\n");
                        exit(EXIT_SUCCESS);
                }
        }
        // its assumed that if there is a none option argument that it is a filepath target for ls relative to .
        else {
            fp = argv[i];
        }
    }

    // get records
    if(opt_U || opt_S) {
        n = scandir(fp, &namelist, NULL, NULL);
        if(opt_S) namelist = bubbleSort(n, namelist, fp);
    }
    else n = scandir(fp, &namelist, NULL, alphasort);
    if (n == -1) {
        printf("filepath: '%s'\n", fp);
        perror("scandir");
        exit(EXIT_FAILURE); 
    }
    
    for(i = 0; i < n; i++) {
        char *name = namelist[i]->d_name;
        if(opt_A && ((name[0] == '.' && !name[1]) || (name[0] == '.' && name[1] == '.'))) {
            
        }else {
            if((name[0] != '.' || (opt_a || opt_A))) {
                if(!opt_l) printf("%s  ", namelist[i]->d_name);
                else {
                    char mode[10] = "";
                    stats = getstats(name, fp);
                    struct tm *info = localtime(&stats.st_atim.tv_sec);
                    parsemode(stats.st_mode, mode);
                    printf("%s %ld %u ", mode, stats.st_nlink, stats.st_uid);
                    if(opt_GID) printf("%u ", stats.st_gid);
                    if(opt_AUTHOR) printf("%u ", stats.st_uid);
                    printf("%ld %d-%d %d:%d %s\n",stats.st_size, info->tm_mon+1, info->tm_mday, info->tm_hour, info->tm_min, name);

                }
            }
        }
        free(namelist[i]);
    }
    
    printf("\n");
    free(namelist);
    
    exit(EXIT_SUCCESS);
} 

    
void parsemode(int modemask, char *mode) {
        if(S_ISREG(modemask)) strcat(mode, "-");
        else strcat(mode, "d");
        if((modemask & S_IRUSR) != 0) strcat(mode, "r");
        else strcat(mode, "-");
        if((modemask & S_IWUSR) != 0) strcat(mode, "w");
        else strcat(mode, "-");
        if((modemask & S_IXUSR)!= 0) strcat(mode, "x");
        else strcat(mode, "-");
        if((modemask & S_IRGRP) != 0) strcat(mode, "r");
        else strcat(mode, "-");
        if((modemask & S_IWGRP) != 0) strcat(mode, "w");
        else strcat(mode, "-");
        if((modemask & S_IXGRP) != 0) strcat(mode, "x");
        else strcat(mode, "-");
        if((modemask & S_IROTH) != 0) strcat(mode, "r");
        else strcat(mode, "-");
        if((modemask & S_IWOTH) != 0) strcat(mode, "w");
        else strcat(mode, "-");
        if((modemask & S_IXOTH) != 0) strcat(mode, "x");
        else strcat(mode, "-");
}


struct stat getstats(char *name, char *fp) {
    char filename[256] = "";
    struct stat stats;
    strcat(filename, fp);
    strcat(filename, "/");
    strcat(filename, name);
    stat(filename, &stats);
    return stats;
}


struct dirent **bubbleSort(int nElems, struct dirent **namelist, char *fp) {
    int i, j;
    for(i = nElems-1; i > 0; i--) {
        for(j = 0; j < i; j++) {
            if(getstats(namelist[j]->d_name, fp).st_size < getstats(namelist[j+1]->d_name, fp).st_size) swap(j, j+1, namelist);
        }
    }
    return namelist;
}


struct dirent **swap(int i, int j, struct dirent **namelist) {
        struct dirent *temp = namelist[i];
        namelist[i] = namelist[j];
        namelist[j] = temp;
        return namelist;
    }
    
    
void help() {
    printf("USAGE ls [OPTION] [FILE]\n");
    printf(" -h, --help         print help message\n");
    printf(" -a, --all          do not ignore entries starting with .\n");
    printf(" -A, --almost-all   do not ignore entries starting with . except '.' and '..'\n");
    printf(" -l, --long         ls with additional stats\n");
    printf("     --author       with -l, print the author of each file\n");
    printf("     -G             with -l, print the author of each file\n");
    printf(" -U                 do not sort, print in directory order\n");
    printf(" -S                 sorts by file size descending using bubblesort\n");
}
