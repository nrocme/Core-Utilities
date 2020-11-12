#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/utsname.h>

int main(int argc, char *argv[])
{
    
    
    struct utsname buf;
    
    if (uname(&buf) == -1)
    {
        perror("uname");
        exit(EXIT_SUCCESS);
    } 
    if(argv[1])
    {
        if (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "--kernel-name") == 0) 
        {
            printf("%s\n", buf.sysname);
        } 
        else if (strcmp(argv[1], "-n") == 0 || strcmp(argv[1], "--nodename") == 0)
        {
            printf("%s\n", buf.nodename);
        }
        else if (strcmp(argv[1], "--help") == 0)
        {
            printf("Usage: uname [OPTION]...\nPrint certain system information.  With no OPTION, same as -s.\n\n  -a, --all                print all information, in the following order,\n  -s, --kernel-name        print the kernel name\n  -n, --nodename           print the network node hostname\n  -r, --kernel-release     print the kernel release\n  -v, --kernel-version     print the kernel version\n  -m, --machine            print the machine hardware name\n  --help     display this help and exit\n\n");
        }
        else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--kernel-version") == 0)
        {
            printf("%s\n", buf.version);
        }
        else if (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "--kernel-version") == 0)
        {
            printf("%s\n", buf.release);
        }
        else if (strcmp(argv[1], "-m") == 0 || strcmp(argv[1], "--machine") == 0)
        {
            printf("%s\n", buf.machine);
        }
        else if (strcmp(argv[1], "-a") == 0 || strcmp(argv[1], "--all") == 0)
        {
            printf("%s\n", buf.sysname);
            printf("%s\n", buf.nodename);
            printf("%s\n", buf.release);
            printf("%s\n", buf.version);
            printf("%s\n", buf.machine);
        }
        else
        {
            printf("Try './uname --help' for more information.\n");
        }
    }
    else 
    {
        (printf("%s\n", buf.sysname));
    }
}
