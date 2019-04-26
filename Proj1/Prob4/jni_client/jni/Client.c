#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int queue = 0;
unsigned second=1;

void *wait(void *sockfd);
int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];
	portno = 2050;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("ERROR opening socket");
		exit(1);
	}
	server = gethostbyname("127.0.0.1");
	if (server == NULL)
	{
		printf("ERROR, no such host");
		exit(0);
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("ERROR connecting");
		exit(1);
	}
	printf("Please enter the message:\n" );

    ////////////////////////////////////////////

	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&thread, &attr, wait, (void *)&sockfd);
	while(1)
	{
		bzero(buffer, 256); //allocate space
		gets(buffer);
		write(sockfd, buffer, strlen(buffer));
		sleep(second);  //wait 
		if (strcmp(buffer, ":q") == 0) //exit
    	{
    		printf("Clinet closing...");
    		close(sockfd);
    		return 0;
    	}
    	else
    	{
    		pthread_mutex_lock(&mutex1);
    		if(queue == 1)				//could receive massage
    		{
    			bzero(buffer,256);
    			n = read(sockfd, buffer, 255);
    			printf("From server:%s\n", buffer);
    		}
    		else				//must be waiting
    			printf("From server: Please wait...\n");
   			pthread_mutex_unlock(&mutex1);
    	}
	}

	////////////////////////////////////////////////

	return 0;
}

void *wait(void *sockfd)
{
	char buffer[256];
	int _sockfd = (int)(*((int*)sockfd));
	int n = read(_sockfd, buffer, 255);	
	pthread_mutex_lock(&mutex1);
   	queue++;
   	pthread_mutex_unlock(&mutex1);
	return 0;
}