/**
	Developed by: Parshwa Shah (110021970)
	version: 1.0
	Advanced System Programmin
	Final Project

	Aim: Server will be running before any client
		 request arrives. server will server the
		 client's request of upload or download
		 the file. it can serve multiple client at
		 same time.
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

void child(int);

/*
	Execution of program starts from main method.
	input: argc is argument count,
		   argv is argument value
		It is used as command line arguments
*/
int main(int argc, char *argv[]){  

	//declaring the required varaible by server
	int serverDiscriptor, client, portNumber, status;
	struct sockaddr_in servAdd;
	
	//Check whether the port number is passes or not
	if(argc != 2){
		printf("Call model: %s <Port Number>\n", argv[0]);
		exit(0);
	}
	if ((serverDiscriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "Cannot create socket\n");
		exit(1);
	}
	
	//Configuring the network parameters to connect with client
	serverDiscriptor = socket(AF_INET, SOCK_STREAM, 0);
	servAdd.sin_family = AF_INET;
	servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
	sscanf(argv[1], "%d", &portNumber);
	servAdd.sin_port = htons((uint16_t)portNumber);

	bind(serverDiscriptor, (struct sockaddr *) &servAdd, sizeof(servAdd));
	listen(serverDiscriptor, 5);//Server is on and running

	printf("\nServer is running...\nWaiting for client to connect...\n");

	//Loop infinitely and wait for client
	while(1){

		//wait until client is connected
		client = accept(serverDiscriptor, NULL, NULL);
		printf("\nClient connected\n");

		//fork the porcess and serve the client's request
		if(!fork()){
			child(client);
		}

		close(client);//Close the connection with client
		waitpid(0, &status, WNOHANG);
	}
	return 0;
}

/**
	child method handles the put and get request of client

	Input: serverDiscriptor is used to read 
		   and write data with client soceket
**/
void child(int serverDiscriptor){

	char *sendMessage,message[255];

	sendMessage="Connection established to server.";
	//printf("%s\n",sendMessage);
	
	//Acknowledge the client of successfull connection
	write(serverDiscriptor, sendMessage, strlen(sendMessage));
	
	//Loop until client terminate the connection
	while(1){

		int i=0,n,file_read,file_write;

		printf("\nWaiting for client to send command\n");
		char message[255] = {};
		
		//Read the command sent by the client
		if(n=read(serverDiscriptor, message, 255)){
		
			message[n]='\0';
			printf("command = %s\n", message);

			//Variables used to store command and filename
			char *pch,*command=NULL,*fileName=NULL;

			//split the string 
			pch = strtok(message," ");
			i=0;

			//Loop used to differentiate command and filename
			while (pch != NULL && i < 2){
				if (i == 0)
					command = pch;
				else
					fileName = pch;

				i++;
				pch = strtok(NULL, " ");
			}

			//print the command and filename entered by the user
			printf("%s\n",command);
			printf("%s\n",fileName);
			char name[255]={};
			sprintf(name,"%s",fileName);

			//Terminate the connection with client if command is quit
			if(!strcasecmp(command,"quit")){
			  printf("\nClient Exited.\n");
			  close(serverDiscriptor);
			  exit(0);
			}
			else if(!strcasecmp(command,"get")){//This block allows client to download file

			    printf("Received get command from client\n");
			    
			    //check whether file to be download exists or not
			    if( access( fileName, F_OK ) != -1 ) {
			    	
			    	//printf("FIle exists\n");
			    	write(serverDiscriptor,"done",4);
			    	printf("File is found and being transfered to client\n");

			    	//Open the file to be downloaded
			    	file_read = open( fileName, O_RDONLY);
			    	i=0;
			    	int fileSize;
			    	char size[255];
			    	
			    	fileSize = lseek(file_read, (off_t)0, SEEK_END);//Find the size of file
			    	lseek(file_read,0L,SEEK_SET);//seek to the starting of the file

			    	sprintf(size,"%d",fileSize);
			    	//printf("size=%s\n",size);

			    	//write the filesize to client
			    	write(serverDiscriptor,size,strlen(size));
			    	
			    	//dynamic memmory allocation
			    	char *buffer_read = (char*)malloc((fileSize));
			    	
			    	//read the data into the buffer
			    	read(file_read,buffer_read,fileSize);
			    	//printf("data=%s\n",buffer_read);
			    	//printf("fileSize= %d\n", strlen(buffer_read));

			    	//write the file to the client
			    	write(serverDiscriptor,buffer_read,fileSize);

			    	printf("File transfered to client successfully.\n");

			    	free(buffer_read);//free the space occupied in heap by buffer_read
			    	close(file_read);//close the file connection

			    } 
			    else {

			    	//This blocks handles the program if file is not present
					write(serverDiscriptor,"undone",4);
					printf("file specified by client does not exists\n");
					continue;
			    }
			}
			else if(!strcasecmp(command,"put")){// handles the upload process for client

				int fileSize;
				printf("Received put command from client\n");
				char message[]={};

				//read tbe messeges from the client
				if(n=read(serverDiscriptor, message, 255)){

				    printf("filename=%s\n",name);    

				    message[n]='\0';
				    printf("message%s\n",message);

				    if(! (strcasecmp(message,"done")) ){

				    	printf("file is ready to upload...\n");
				    	char message[255]={};
				    	read(serverDiscriptor,message,255);
						fileSize = atoi(message);
						//printf("filesize= %d\n", fileSize);

						//create or open file to be saved
						file_write  = open(name, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
						//printf("filename=%s\n",name);
						char *buffer_read = (char*)malloc((fileSize));
						
						//read the data uploaded by the user
						read(serverDiscriptor,buffer_read,fileSize);
						//printf("data=%s\n",buffer_read);

						//write the data to the local file
						write(file_write,buffer_read,fileSize);

						printf("File uploaded successfully.\n");

						//Free up the space and close the connection of file
						free(buffer_read);
						close(file_write);
				    }
				    else{
				    	printf("File not present on client side\n");
			      		continue;
				    }
				}
			}
			else{
				printf("Invalid command from client\nTerminating client connection");
				continue;
			} 
		}
		else{

			//if client terminates by entering quit command
			printf("client exited.\n");
			close(serverDiscriptor);
			exit(0);
		}
	}

}