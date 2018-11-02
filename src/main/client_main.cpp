#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int fileDesc, portNo, n;
    struct sockaddr_in serverAdd;
    struct hostent *server;
    char buffer[256];
    map <string, int> myConnections;

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
    	exit(0);
    }

    portNo = atoi(argv[2]);
    //returns pointer to hostent
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}


	// Not sure of the pointers work
	size_t len1 = strlen(argv[2]);
	size_t len2 = strlen((char *)server->h_addr);
	char *key = malloc(len1 + len2 + 1);
	if (!key) abort();
	memcpy(key,argv[2], len1);
	totalLine[len1] = '-';
	memcpy(totalLine + len1,server->h_addr, len2);
	totalLine[len1 + len2] = '\0';

 //   if(myConnections.find(key) != myConnections.end() && RealConnection) {
    if(myConnections.find(key) != myConnections.end()) {

        fileDesc = myConnections[key];
        if (connect(fileDesc,(struct sockaddr *)&serverAdd,sizeof(serverAdd)) == -1)
                error("ERROR connecting");
        //read file because we are already connected
    }

    else {
    fileDesc = socket(AF_INET, SOCK_STREAM, 0);
      if (fileDesc < 0)
          error("ERROR opening socket");
    myConnections[key] = fileDesc;
    //sets all values in a buffer to zero
    bzero((char *) &serverAdd, sizeof(serverAdd));

    //setting server attributes
    serverAdd.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serverAdd.sin_addr.s_addr,
         server->h_length);
    serverAdd.sin_port = htons(portNo);

    if (connect(fileDesc,(struct sockaddr *)&serverAdd,sizeof(serverAdd)) == -1)
        error("ERROR connecting");
    //read file
    }

//    printf("Please enter the message: ");
//    bzero(buffer,256);
//    fgets(buffer,255,stdin);
//    n = write(sockfd,buffer,strlen(buffer));
//    if (n < 0)
//         error("ERROR writing to socket");
//    bzero(buffer,256);
//    n = read(sockfd,buffer,255);
//    if (n < 0)
//         error("ERROR reading from socket");
//    printf("%s\n",buffer);
    return 0;
}
