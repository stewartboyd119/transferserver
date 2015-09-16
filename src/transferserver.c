#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>


#if 0
/*
 * Structs exported from netinet/in.h (for easy reference)
 */

/* Internet address */
struct in_addr {
  unsigned int s_addr;
};

/* Internet style socket address */
struct sockaddr_in  {
  unsigned short int sin_family; /* Address family */
  unsigned short int sin_port;   /* Port number */
  struct in_addr sin_addr;	 /* IP address */
  unsigned char sin_zero[...];   /* Pad to size of 'struct sockaddr' */
};

/*
 * Struct exported from netdb.h
 */

/* Domain name service (DNS) host entry */
struct hostent {
  char    *h_name;        /* official name of host */
  char    **h_aliases;    /* alias list */
  int     h_addrtype;     /* host address type */
  int     h_length;       /* length of address */
  char    **h_addr_list;  /* list of addresses */
}
#endif

#define BUFSIZE 4096

#define USAGE                                                                 \
"usage:\n"                                                                    \
"  transferserver [options]\n"                                                \
"options:\n"                                                                  \
"  -p                  Port (Default: 8888)\n"                                \
"  -f                  Filename (Default: bar.txt)\n"                         \
"  -h                  Show this help message\n"
void open_file_and_write (int sock, char * filename);
void error(const char *msg);
int main(int argc, char **argv) {
  int option_char;
  int portno = 8888; /* port to listen on */
  char *filename = "bar.txt"; /* file to transfer */

  // Parse and set command line arguments
  while ((option_char = getopt(argc, argv, "p:f:h")) != -1){
    switch (option_char) {
      case 'p': // listen-port
        portno = atoi(optarg);
        break;
      case 'f': // listen-port
        filename = optarg;
        break;
      case 'h': // help
        fprintf(stdout, "%s", USAGE);
        exit(0);
        break;
      default:
        fprintf(stderr, "%s", USAGE);
        exit(1);
    }
  }

  /* Socket Code Here */
  int sockfd, newsockfd;
  //char buffer[256];
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
	  error("ERROR opening socket");
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1){
	  error("Error on setting options");
   }
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
          error("ERROR on binding");
  }
  if (listen(sockfd,5) == -1){
	  error("ERROR on listen");
  }
  clilen = sizeof(cli_addr);

  newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
  if (newsockfd < 0){
	  error("ERROR on accept");
  }
  open_file_and_write(newsockfd, filename);

  close(newsockfd);
  close(sockfd);
  return EXIT_SUCCESS;

 }

void open_file_and_write (int sock, char * filename)
{

    int fhandle_open;
    char buffer[1];

    fhandle_open = open(filename, O_RDONLY, S_IREAD);
    if (fhandle_open == -1){
    	error("Error opening file: %s");
    }

    bzero(buffer,sizeof(buffer));
    while (read(fhandle_open, (void *)buffer, sizeof(buffer)) != 0){
		if (write(sock, (void *)buffer, sizeof(buffer)) == -1){
			error("ERROR writing to socket");
    	}
    }
    close(fhandle_open);
}
void error(const char *msg){
	perror(msg);
	exit(1);
}
