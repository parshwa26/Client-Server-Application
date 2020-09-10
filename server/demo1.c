/**

    Developed by : parshwa shah 110021970

    Version: 1.0

    Lab 2 of Advances System Programming

    PROGRAM AIM: This program takes two inputs one is filename and another is directory.
                 Filename is the name of file to be searched in the given directory.
                 This two argument can be given using command line argument and if not
                 provided default value is selected. If file is found then it is
                 didplayed its path.
*/

#define _XOPEN_SOURCE 700
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <limits.h>
#include <dirent.h>

char *fileName;
int fileFound = 1;

/**
    This function checks current and all sub directories recusively

    parameter: directory name is the name of current directory to be searched
               filename is the name of file to be searched
*/
int checkFile(const char *filePath, const struct stat *st,int typeFlag, struct FTW *ftwBuffer)
{
    if(strcmp(filePath+ftwBuffer->base,fileName)==0)
    {
        if(typeFlag == 1)
        {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", filePath,"");

            char buffer[PATH_MAX]; // PATH_MAX incudes the \0 so +1 is not required
            char *res = realpath(path, buffer);
            if (res)
                printf("\nFound: %s is directory and located at %s \n", fileName,buffer);
            else
                perror("realpath");
        }
        if(typeFlag == 0)
            printf("\nFound: File named %s in %s\n", fileName, filePath);

        fileFound = 0;
    }
    return 0;
}

//Execution of program starts from main function
int main(int argumentCount, char *argumentValue[])
{
    char *targetDirectory = "./";
    if(argumentCount == 1)
    {
        printf("No arguments are provided to search file. Default value is selected to search file.\n");
        printf("No arguments are provided for directory. Default value of ./ is selected to search directory.\n\n");
        fileName = "110021970_Assignment2.c";
    }
    else if(argumentCount == 2)
    {
        printf("No arguments are provided for directory.\nDefault value of ./ is selected to search directory.\n\n");
        fileName = argumentValue[1];

    }
    else
    {
        fileName = argumentValue[1];
        targetDirectory = argumentValue[2];
    }

    if (nftw(targetDirectory, checkFile, 25,0)== -1)
    {
        perror("function Call to nftw is failed");
        exit(0);
    }
    if(fileFound)
        printf("No such file or directory/subdirectory exists\n");

    return 0;
}
