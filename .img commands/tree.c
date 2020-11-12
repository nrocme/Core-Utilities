#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "fs.h"


int lread(int bpos, void *buf, int size);
int readino(int ino, struct dinode *di);
char *readfile(struct dinode di, char *buf);
int tree(int ino, int level);
int getblock( int blkno, void *block, int size);
struct superblock sb;
int fsfd;
char *type[] = {"?", "DIR ", "FILE", "DEV "};


int main(int argc, char *argv[])
{
    int buf[BSIZE];
    if (argc < 2) {
        printf("Usage:tree <image>\n");
        return 1;
    }
    else {
        fsfd = open(argv[1], O_RDONLY);
        getblock(1, buf, sizeof(sb));
        memcpy(&sb, buf, sizeof(sb));
   }
    tree(1, 0);
    return 0;
}


int getblock(int blkno, void *block, int size)
{
    if (lseek(fsfd, blkno*BSIZE, SEEK_SET) != blkno*BSIZE) {
        perror("lseek");
        exit(0);
    }
    return read(fsfd, block, size);
}

/**
 * Lists the directory given by ino, level determines the indentation level.
 */
int readino(int ino, struct dinode *di)
{
  int inoloc = (sb.inodestart*BSIZE)+(ino*sizeof(struct dinode));
  if (lseek(fsfd, inoloc, SEEK_SET) != inoloc) {
    perror("lseek");
    return -1;
  }  
  return read(fsfd, di, sizeof(struct dinode));
}


int tree(int ino, int level)
{
    int i, j;
    struct dinode di;
    readino(ino, &di);
    struct dirent buf[di.size/sizeof(struct dirent)];
    getblock(di.addrs[0], &buf, di.size);
    for(i = 0; i < (sizeof(buf)/sizeof(struct dirent)); i++) {
        if(buf[i].inum == 0 || strncmp(buf[i].name, ".", 1) == 0 || strncmp(buf[i].name, "..", 2) == 0) continue;
        else {
            readino(buf[i].inum, &di);
            for(j = 0; j < level; j++) printf("    ");
            printf("%s %d %d %s\n", type[di.type], buf[i].inum, di.size, buf[i].name);
            if(di.type == 1) tree(buf[i].inum, level+1);
        }
    }
    return 1;
}
/**
 * Read the entire file given by the inode di into memory, returning it.
 * "size" is optional, but set to the size of the file read.
 */
// char *readfile(struct dinode di, char *buf)
// {
//     // prograsm in for files that needs more then 13 block
//     int sz = 0, blk = 0;
//     while (sz < di.size) {
//         if(di.addrs[blk] != 0){
//             getblock(di.addrs[blk], buf+sz);
//             //printf("%.*s", MIN(BSIZE,di.size - sz), buf+sz);
//             blk++;
//             sz += BSIZE;
//         } else break;
//     }
//     buf[di.size] = '\0';
//     return buf;
// }
