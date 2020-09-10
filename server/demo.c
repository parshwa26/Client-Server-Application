/**

    Developed by : parshwa shah 110021970

    Version: 1.0

    Lab 1 of Advances System Programming

    PROGRAM AIM: Develop a C program in to take name from
    command line argument. Search that name in current directory
    and all its sub directory. If name is found display the name
    and also specify whether it is file or directory name
*/

#include<stdio.h>
#include<dirent.h>
#include<string.h>

int fileFound = 1;

//Execution of program starts from main function
int main(int argumentCount, char *argumentValue[])
{
    // If command line argukent is not specified default value of 110021970_lab_1.c is taken
    if(argumentValue[1] == NULL)
    {
        printf("No arguments are provided to search file.\nDefault value is selected to search file.\n");
        argumentValue[1] = "110021970_lab_1.c";
    }

    // call to user defined function to check whether name exists or not
    checkFile(".",argumentValue[1]);

    // If name is not found show the message
    if(fileFound)
        printf("No such file or directory/subdirectory exists\n");

    return 0;
}

/**
    This function checks current and all sub directories recusively

    parameter: directory name is the name of current directory to be searched
               filename is the name of file to be searched
*/
int checkFile(char *directoryName[], char *filaName[])
{

    // DIR varaible points to files/direcory
    DIR *folder;
    struct dirent *entry;
    folder = opendir(directoryName);

    // If given directory is not found display the error message
    if(folder == NULL)
    {
        printf("Error: In opening %s directory\n", directoryName);
        return 1;
    }

    // Read all the files and folder of the directory
    while( entry = readdir(folder))
    {
        if(entry->d_type == DT_DIR && (strcmp(entry->d_name,".")) && (strcmp(entry->d_name,"..")))
        {
            // Recursive call for all the sub directories
            checkFile(entry->d_name,filaName);
        }

        //compare the user inputed name and name readed from the directory
        if(!strcmp(entry->d_name,filaName))
        {
            if(entry->d_type == DT_DIR)
                printf("%s found and it is directory... \n", filaName);
            else
                printf("File named %s found in %s directory... \n", filaName, directoryName);
            fileFound = 0;
            return 0;
        }
    }

    //close the opened directory
    closedir(folder);
    return 1;
}
