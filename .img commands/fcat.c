#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "fs.h"

int findino(int index, int ino);
void readfile(int ino, void *buf);
int readino(int ino, struct dinode *di);
int lread(int bpos, void *buf, int size);
int getblock(int blkno, void *block, int size);
// The superblock and file-system file descriptor:
struct superblock sb;
int fsfd;
char paths[BSIZE][BSIZE];
#define MIN(a,b)	((a) < (b)? (a) : (b))
int main(int argc, char *argv[])
{
    int i = 0;
    char *p;
    if (argc < 3) {
        printf("Usage:fcat <image> <file>\n");
        return 1;
    }
    
    for(p = strtok(argv[2], "/"); p != NULL; p = strtok(NULL, "/")) {
        memcpy(paths[i++], p, strlen(p));
    }
    
    int ibuf[BSIZE];
    fsfd = open(argv[1], O_RDONLY);
    getblock(1, ibuf, sizeof(sb));
    memcpy(&sb, ibuf, sizeof(sb));
    int ino = findino(0, 1);
    if(ino == -1) {
        printf("FILE NOT FOUND\n");
        exit(0);
    };
    struct dinode di;
    readino(ino, &di);
    char buf[di.size];
    readfile(ino, buf);
    printf("%s", buf);
    return 0;
}

/**
 * This function is recursive, given an array of paths, it scans for the dir/file
 * of path[index] in the directory given by the inode number 'ino'.  Returns the
 * inode of the file at the end of the paths[] array.
 */
int findino(int index, int ino)
{   
    struct dinode di;
    readino(ino, &di);
    struct dirent dirs[di.size/sizeof(struct dirent)];
    readfile(ino, dirs);
    for(int i = 0; i < di.size/sizeof(struct dirent); i++){
        if(strcmp(dirs[i].name, paths[index]) == 0){
           if(strcmp(paths[index+1], "") == 0) return dirs[i].inum;
           return findino(index+1, dirs[i].inum); 
        }
    }
    return -1;
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
 * Returns the inode structure for inode number 'ino'
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

/**
 * Reads a file given by the inode number 'ino' from the filesystem and returns
 * the data of the file.
 */
void readfile(int ino, void *buf)
{
    struct dinode di;
    readino(ino, &di);
    int sz = 0, blk = 0;
    while (sz < di.size) {
        getblock(di.addrs[blk], buf+sz, MIN(BSIZE,di.size - sz));
        blk++;
        sz += BSIZE;
    }
}

