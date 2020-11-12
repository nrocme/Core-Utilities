#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

long int findSize(char fileName[]) 
{ 
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

int main(int argc, char *argv[])
{
    // gets path from the argument after ./a.out so to use cp you would need to ./a.out <pathname of copy> <where to put the copy>
    char *pathSRC = argv[1];
    char *pathDEST = argv[2];
    int bytesRead = 0;
    
    //call findsize to get the required size of buffer
    long int size = 0;
    size = findSize(argv[1]);
    void *buffer = malloc(size);
    
    if (!argv[1])
    {
        printf("Missing Arguments ./cp <SourceFilePath> <DestinationFilePath>\n './cp --help' for more information\n");
        return 0;
    }
    else if(strcmp(argv[1],"--help") == 0)
    {
        printf("cp takes a source file and writesit to a destination file if the\ndestination file does not exist it will be created and written to\n format ./cp <SourceFilePath> <DestinationFilePath>\n");
        return 0;
    }
    else if(argv[1] && argv[2])
    {
        //File descriptors for both the SRC and the DEST file DEST is created upon command use.
        int SRC = open(pathSRC, O_RDONLY);
        int DEST = open(pathDEST, O_WRONLY | O_CREAT , 00666);
        
        if (SRC == -1 || DEST == -1)
	   {
            printf("Error Number %d/n", errno);
            perror("Program");
            return 0;
	   }
        // read SRC store bytesRead holds how many bytes were read
        bytesRead = read(SRC, buffer, size);
    
        // Writes SRC from buffer to DEST
        write(DEST, buffer, bytesRead);
        printf("%s copied to %s successfully\n", argv[1] , argv[2]);
        return 0;
    }
    
    // catch all error for if just on pathname is passed adn the entire loop is avoided maybe such as typing --help with error i.e : cp help or cp -help
    printf("Missing Arguments ./cp <SourceFilePath> <DestinationFilePath>\n './cp --help' for more information\n");
    
    return 0;
}
