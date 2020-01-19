#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "sockop.h"
using namespace std;
#define BUFSIZE	1024

int main(int argc, char *argv[])
{
	int connfd;	/* socket descriptor */
	int n;
	string x;
	string s;
	char bufw[BUFSIZE], bufr[BUFSIZE];
	connfd = connectsock(argv[1], argv[2], "tcp");
	while(1) {
        memset(bufr, 0, BUFSIZE);
		if ((n = read(connfd, bufr, BUFSIZE)) == -1)
			errexit("Error: read()\n");
		printf("Server Reply: %s\n", bufr);
		cout<<"***"<<bufr[strlen(bufr)-2]<<endl;
		/* write message to server */
        s = "";
		getline(cin,s);
		sprintf(bufw, "%s", s.c_str());
		if ((n = write(connfd, bufw, strlen(bufw))) == -1)
			errexit("Error: write()\n");
		/* read message from the server and print */
		sleep(1);
	}
	/* close client socket */
	close(connfd);

	return 0;
}
