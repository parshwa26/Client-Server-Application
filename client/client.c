/**
	Developed by: Parshwa Shah (110021970)
	version: 1.0
	Advanced System Programmin
	Final Project

	Aim: Client file is used to give commands to server.
		 command can be of download, upload or quit the
		 program. If it is download than it downloads
		 file from server and stores at client side.
		 If command is upload it will upload file to 
		 server and server will save it. This process 
		 continues unit user enter quit command.
**/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

/*
	The execution of the program starts from
	tha main method
*/
int main(int argc, char *argv[]){

	//Variabel declaration and initialization
	char message[255],character;
	int server, portNumber, n;
	struct sockaddr_in servAdd;
	int i=0;

	// Return error if two command line arguments are not passed
	if(argc != 3){
		printf("Call model: %s <IP Address> <Port Number>\n", argv[0]);
		exit(0);
	}

	//Check if socket is created successfully or not
	if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "Cannot create socket\n");
		exit(1);
	}

	//Configure the TCP network protocol parameters
	servAdd.sin_family = AF_INET;
	sscanf(argv[2], "%d", &portNumber);
	servAdd.sin_port = htons((uint16_t)portNumber);

	if(inet_pton(AF_INET, argv[1], &servAdd.sin_addr) < 0){
		fprintf(stderr, " inet_pton() has failed\n");
		exit(2);
	}

	//Check whether the connectin between client and server is established or not
	if(connect(server, (struct sockaddr *) &servAdd, sizeof(servAdd))<0){
		fprintf(stderr, "connect() has failed, exiting\n");
		exit(3);
	}

	printf("Enter identical commands from the below list\n");
	printf("--------------------------------------------\n");
	printf("1) put <fileName>\n");
	printf("2) put a.txt\n");
	printf("3) get <fileName>\n");
	printf("4) get demo.c\n");
	printf("5) quit\n\n");
	//read the message sent by server
	if(n=read(server, message, 255)){
	    message[n]='\0';
	    fprintf(stderr,"%s\n", message);
	}

	//Loop unit client enter quit command
	while(1){

		//Declaraing and initializing various required variable
		char message[255]={};
		printf("\nEnter command>\n");

		//dynamic memory allocation
		char *buffer_read_command = (char*)malloc((1000+1) * sizeof(char));
		int fileSize,file_write,file_read;
		buffer_read_command[0] = '\0';
		bzero(buffer_read_command,1000);
		i=0;

		//Read the command entered by the user
		while (read(STDIN_FILENO, &character, sizeof(char)) == 1) {
			if (character=='\n') 
				break;
			buffer_read_command[i] = character;
			i++;
		}

		//If command entered is quit than terminate the client program
		if(!strcasecmp(buffer_read_command,"quit")){
			write(server,buffer_read_command,strlen(buffer_read_command));//Inform server about quit command
			printf("Bye Bye...\nconnection terminated.");
			close(server);//Terminates the connection to server
			exit(0);
		}

		//printf("%s\n", buffer_read_command);

		//required variable for storing filename and command
		char *pch =NULL,*command = NULL,*fileName="\0",sendMsg[255]={};

		//Split the command using space
		pch = strtok(buffer_read_command," ");
		i=0;
		while (pch != NULL && i < 2){
			if (i == 0)
				command = pch;//Stores the get or put command
			else
				fileName = pch;//stores the filename to the variable

			i++;
			pch = strtok(NULL, " ");
		}

		//printf("cmd=%s\n",command);
		//printf("filename=%s\n",fileName);

		//Validating command entered by the user
		if( !(strcmp(fileName,"\0"))){
			printf("\nNo file name passed\n");
			printf("Command syntax: get fileName OR put fileName OR quit \n");
			continue;
		}
		
		sprintf(sendMsg,"%s %s",command,fileName);
		printf("command=%s\n",sendMsg);

		//sendin the command to the server
		write(server,sendMsg,strlen(sendMsg));

		//if file download is the request follow this block
		if(!strcasecmp(command,"get")){
			
			//printf("get\n");
			char message[255] = {};

			if(n=read(server, message, 255)){
			    message[n]='\0';

			    if(! (strcasecmp(message,"done")) ){

	    	  		printf("\nRequesting server for file...\n");
	    			printf("file found get ready to download file\n");
	    			//int file_write  = open( fileName, O_CREAT | O_WRONLY | O_TRUNC, 0755);
	    			
	    			//Create or open the file to be downloaded
	    			file_write  = open(fileName, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
	    			char message[255] = {};
	    			read(server,message,255);
					fileSize = atoi(message);//Convert filesize from string to integer
					//printf("filesize = %d\n",fileSize);

					//Dynamic memory allocation based on filesize to be downloaded
					char *buffer_read = (char*)malloc((fileSize));
					//printf("hiiiiiii\n");
					read(server,buffer_read,fileSize);
					//printf("%s\n",buffer_read);

					write(file_write,buffer_read,fileSize);//Write data to file = downloading
					printf("File downloaded successfully.\n");

					free(buffer_read);//free the space
					close(file_write);//close the file connection
				}
			    else{
			    	printf("No such file found.\nTry with another fileName");
			    	continue;
			    }
			}
			else{
				printf("Server error\n");
				continue;//skip the below portion of while and goto to starting of while loop
			}
		}else if(!strcasecmp(command,"put")){//Upload file
			
			//printf("put\n");
			
			//Validate whether file Exists or not
			if( access( fileName, F_OK ) != -1 ) {

				command= NULL;
				write(server,"done",4);
				printf("File is found and being transfered to server\n");
				file_read = open( fileName, O_RDONLY);//Open file to be uploaded
				i=0;
				int fileSize;
				char size[255];
				
				fileSize = lseek(file_read, (off_t)0, SEEK_END);//find the size of file
				lseek(file_read,0L,SEEK_SET);//seek the file descriptor to starting

				sprintf(size,"%d",fileSize);
				//printf("%s\n",size);
				write(server,size,strlen(size));//write the filesize to server

				char *buffer_read = (char*)malloc((fileSize));
			    	
			    read(file_read,buffer_read,fileSize);//read content of file to be uploaded
			    //printf("data=%s\n",buffer_read);
			    write(server,buffer_read,fileSize);//write data of file to server
			    
			    printf("File transfered to server successfully.\n");
			    printf("File uploaded.\n");
			    
			    free(buffer_read);//free up the memory occupied in heap
			    close(file_read);//closes the file connection
			}
			else{

				//This blocks handle if file to be upload is not present
				write(server,"undone",6);
				printf("\nFile %s doesnot exists\nTry with another fileName",fileName);
				continue;
			}
		}
		else{

			printf("please enter valid command\n");
			printf("Command syntax: get fileName OR put fileName OR quit \n");
		}
		free(buffer_read_command);//free the occupied space
	}
  return 0;
}