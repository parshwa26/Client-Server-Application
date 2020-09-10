#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

void child(int);

int main(int argc, char *argv[]){  
  int sd, client, portNumber;
  struct sockaddr_in servAdd;     // server socket address
  
 if(argc != 2){
    printf("Call model: %s <Port Number>\n", argv[0]);
    exit(0);
  }

  sd = socket(AF_INET, SOCK_STREAM, 0);
  servAdd.sin_family = AF_INET;
  servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
  sscanf(argv[1], "%d", &portNumber);
  servAdd.sin_port = htons((uint16_t)portNumber);
  
  bind(sd, (struct sockaddr *) &servAdd, sizeof(servAdd));
  listen(sd, 5);

  while(1){
    client = accept(sd, NULL, NULL);
    printf("Got a client\n");

    if(!fork())
      child(client);

    close(client);
  }
}

void child(int sd){
	char message[255];


	while(1){
		fprintf(stderr, "Enter a line to send to client\n");
		fgets(message, 254, stdin);
		write(sd, message, strlen(message)+1);

		if(!read(sd, message, 255)){
			close(sd);
			fprintf(stderr, "Bye, my client is dead, wait for a new one\n");
			exit(0);
		}
		fprintf(stderr, "Clent send back: %s\n", message);
	}
}