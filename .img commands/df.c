#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include "fs.h"

struct superblock sb;
struct dinode di;

int getblock(int fd, int blkno, void *block);

int main(int argc, char *argv[])
{
    if(argc < 2) printf("No image file found\n");
    else {
        for(int i = 1; i < argc; i++){
            int buf[BSIZE];
            int fd = open(argv[i], O_RDONLY);
            char cbuf[BSIZE];
            // getting superblock
            getblock(fd, 1, buf);
            memcpy(&sb, buf, sizeof(sb));
            getblock(fd, sb.bmapstart, cbuf);
            int used = 0;
            for(int i = 0; i < 1000; i++)
                if (((cbuf[i / 8]) & (1 << (i % 8))) > 0) used++;
            printf("FILESYSTEM\tSIZE USED AVAIL USE%\n");
            printf("%-*s\t%-*d %-*d %-*d %0.f\n", 8, argv[i], 4, sb.size, 4, used, 5, sb.size-used, floor(used/((double)sb.size)*100));
        }
    }
    return 1;
}

int getblock(int fd, int blkno, void *block)
{
    if (lseek(fd, blkno*BSIZE, SEEK_SET) != blkno*BSIZE) {
        perror("lseek");
        exit(0);
    }
    return read(fd, block, BSIZE);
}
