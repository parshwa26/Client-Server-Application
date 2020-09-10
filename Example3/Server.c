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


void child(int);

int main(int argc, char *argv[]){  
  int sd, client, portNumber, status;
  struct sockaddr_in servAdd;      // client socket address
  
 if(argc != 2){
    printf("Call model: %s <Port Number>\n", argv[0]);
    exit(0);
  }
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    fprintf(stderr, "Cannot create socket\n");
    exit(1);
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
    printf("Got a client, start chatting\n");
    
    if(!fork())
      child(client);

    close(client);
    waitpid(0, &status, WNOHANG);
  }
}

void child(int sd){
  char message[255];
        int n, pid;


        write(sd, "start chatting", 16);

  pid=fork();
  if(pid)                         /* reading client messages */
          while(1)
     if(n=read(sd, message, 255)){
       message[n]='\0';
       fprintf(stderr,"%s\n", message);
       if(!strcasecmp(message, "Bye\n")){
          kill(pid, SIGTERM);
          exit(0);
       }
     }
  if(!pid)                      /* sending messages to the client  */
     while(1)
      if(n=read(0, message, 255)){
        message[n]='\0';
        write(sd, message, strlen(message)+1);
        if(!strcasecmp(message, "Bye\n")){
           kill(getppid(), SIGTERM);
           exit(0);
              }
      }
}