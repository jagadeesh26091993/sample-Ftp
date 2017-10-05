/*
 Filename : prog6srv.c
 Description : This file contains the program for the server which takes requests from the clients and sends responses to the clients. 
*/

#include "prog6srv.h"
#include "wrap6srv.h"
#include "inOut.h"
//global variable to store the output generated by server
int main(int argc, char *argv[])
{
 // a variable to store port number
  uint16_t portno;
 //socklen_t variable to store the structure length of the client
  socklen_t clilen;
 if(argc != 2)
  {
     portno = SERV_PORT;   
   }
  else
   {
      portno = atoi(argv[1]);
    }
    //listen fd is used to store the return value of socket and connfd is used to store the value of accept function
    int listenfd, connfd;
	pid_t childpid;
    // structure to store socket information
    struct sockaddr_in servaddr,cliaddr;
    //calling the Socket wrapper function inorder to create a socket
    listenfd = Socket(DOMAIN, TYPE, PROTOCOL);
    // filling the structure variable with zeros to get rid of compilation errors
    bzero(&servaddr, sizeof(servaddr));
    //Filling the socket structure with necassary information
     servaddr.sin_family = DOMAIN;
     servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
      servaddr.sin_port = htons(portno);
     //caling the Bind wrapper function to bind it to the socket
     Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
     //calling the Listen wrapper function in order to listen for the clients
     Listen(listenfd, LIST);
	 Signal(SIGCHLD,sig_chld);
     //calling the file wrapper inorder to open a file.
//     FILE* fp = Fopen(PATH,"r");
     //infinite loop to run a server continously.
     for( ; ; )
        {
        clilen = sizeof(cliaddr);          
         //calling the accept function if the server accepts the connection
         connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
		      if((childpid = Fork()) ==0)
			  {
				//closing the listening socket of child process.
				  Close(listenfd);
              //calling the cal function inorder to get the requested data. 
			  cal(connfd);
			//closing the connected socket of child process.
			  Close(connfd);
			   exit(0);
			  }
        //closing the connecting socket of father process.
          Close(connfd);
  }
// closing the listening socket of father process.
  Close(listenfd);
 //Close(connfd);
  //Fclose(fp);
 exit(0);
}
/*
  Function Name : cal
  Return : returns nothing.
  Description : This function will handle the clients request  and gives the response to the clients requests.
*/
  void cal(int connfd)
   {
	   //variable to store the recieved message.
    char recvline[MAXLINE];
	//variable to store the command.
    char* ch1;
    while(Readline(connfd,recvline,MAXLINE))
    {
      // replacing /n with /0.
       recvline[strlen(recvline)-1] ='\0'; 
     //  checking for the command pwd or not.
		if(strcmp(recvline,"pwd")==0)
		{
		printf(" \n");
		//calling the pwd function to get the present working directory.
		ch1 = PWD();
		printf(" \n");
        bzero(recvline,MAXLINE);
		//copying the string.
        strcpy(recvline,ch1);
      //appending \n to the string.
        recvline[strlen(recvline)] = '\n';
	  //writing response to the socket.
		Writen(connfd,recvline,strlen(recvline));
        bzero(recvline,MAXLINE);
	}
	//checking for the cdup command.
        else if(strcmp(recvline,"cdup")==0)
	{
		//filling the array with zeros.
	  bzero(recvline,MAXLINE);
	  //changimg the directory to its parent.
	  int er = chdir("..");
	  //filling the recvline to send send the response.
          if(er==0)
		{
		 recvline[0] = '0';
		recvline[1]='\n'; 
		}
		else
		{
		 recvline[0] ='1';
		recvline[1]='\n';
		}
		//writing the data to the socket.
 	   Writen(connfd,recvline,strlen(recvline));
	   bzero(recvline,MAXLINE);  
	}
	//if tthe command is cd
	else
	{
			//declaring variables to stores the parsed string.
          char* even = NULL; char* cm=NULL; char* cm1 = NULL;
		//splitting the string.
		even = strtok(recvline," ");
         if(even!=NULL)
			 { 
			//allocating memory.
				cm = (char*) malloc(strlen(even)*sizeof(char));
                //cpying the string.
				strcpy(cm,even);
			}
			//splitting the string.
           even = strtok(NULL," ");
			if(even!=NULL)
			{
				//allocating memory.
				cm1 = (char*) malloc(strlen(even)*sizeof(char));
				//copying the string.
				strcpy(cm1,even);
			}
				//comparing with cd.
				if(strcmp(cm,"cd") ==0) 
				{
                       int e;
					   //comparing for ~
                              if((strcmp(cm1,"~") ==0))
								{
									//changing to home directory.
									e = chdir(getenv("HOME"));
								}
                              
			else
				{
					//changing the directory to the specified path.
				e = chdir(cm1);
				}
				//filling the recvline with the data that we want to send.
					if(e==0)
					{
					 recvline[0] = '0';recvline[1]='\n';
					}
					else
					{
					recvline[0]='1';recvline[1]='\n';
					}
					//writing the data to the socket.
				Writen(connfd,recvline,strlen(recvline));
				bzero(recvline,MAXLINE);
				 }
	}
}
}
 // function used to avoid the zombie processes.  
void sig_chld()
{

	int stat;
	//calling the waitpid function in a loop to handle the terminated child processes.
	 Wait(&stat);
}
