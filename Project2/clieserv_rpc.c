#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

struct hostent *gethostbyname() ;
void client(char *host, int port, int datasize);
void server(int port, int datasize);
void printSA(struct sockaddr_in fred);
void makeDestSA(struct sockaddr_in *sa, char *hostname, int port);
void makeLocalSA(struct sockaddr_in *sa);
long time_diff(struct timeval *tp2, struct timeval *tp1);

#define	NTIMES	1024

/**
 * Act as a client or server, exchanging UDP messages
 */
main(int argc, char **argv)
{
	int 	port, datasize, error;
	struct timeval	start, end;

	port = getuid() + IPPORT_RESERVED;
	datasize = 0;
	error = -1;

	gettimeofday(&start, NULL);
	switch(argc)	
	{
	case 3:
		if(argv[1][0] == 's') 
		{
			sscanf(argv[2], "%d", &datasize);
			if(datasize > 0)	
			{
			   server(port, datasize);
			   error = 0;
			}
		}
		break;

	case 4:
		if(argv[1][0] == 'c')	
		{
			sscanf(argv[3], "%d", &datasize);
			if(datasize > 0)	
			{
			   client(argv[2], port, datasize);
			   error = 0;
			}
		}
		break;

	default:
		break;
	}

	if(error)	
	{
		printf("Usage: %s c/lient machine datasize (> 0) or\n", argv[0]);
		printf("%s s/erver datasize (>0)\n", argv[0]);
	} 
	else	
	{
		gettimeofday(&end, NULL);
		printf("time in microseconds = %d\n", time_diff(&end, &start));
		printf("time in microseconds per rpc = %d\n", time_diff(&end, &start)/NTIMES);
	}
}

/*print a socket address */
void printSA(struct sockaddr_in sa)
{
	printf("sa = %d, %s, %d\n", sa.sin_family,
		inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
}

/* make a socket address for a destination whose machine and port are given as arguments */
void makeDestSA(struct sockaddr_in *sa, char *hostname, int port)

{
	struct hostent *host;

	sa->sin_family = AF_INET;
	if((host = gethostbyname(hostname))== 0)
	{
		perror("Unknown host name");
		exit(-1);
	}
	sa-> sin_addr = *(struct in_addr *) (host->h_addr);
	sa->sin_port = htons(port);
}

/* make a socket address using any of the addressses of this computer
for a local socket on any port */
void makeLocalSA(struct sockaddr_in *sa)

{

	sa->sin_family = AF_INET;
	sa->sin_port = htons(0);
	sa-> sin_addr.s_addr = htonl(INADDR_ANY);
}

static char *msg = 0;

/**
 * Act as a server, expecting UDP messages of given size on given port
 */
void server(int port, int datasize)
{
	char *msg;
	struct sockaddr_in mySocketAddress, aSocketAddress;
	int s, aLength, n, reply;
	register int loop;

	if((msg = (char *)malloc(datasize)) == 0)	
	{
		perror("server: not enough memory!");
		return;
	}

	if((s = socket(AF_INET, SOCK_DGRAM, 0))<0) 
	{
		perror("server: socket failed");
		return;
	}

	mySocketAddress.sin_family = AF_INET;
	mySocketAddress.sin_port = htons(port);
	mySocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	if( bind(s, &mySocketAddress, sizeof(struct sockaddr_in))!= 0)
	{
		perror("server: Bind failed\n");
		close(s);
		return;
	}

	/*printSA(mySocketAddress);*/
	aLength = sizeof(aSocketAddress);
	aSocketAddress.sin_family = AF_INET;
	for(loop = 0; loop < NTIMES; ++loop)	
	{
		n = recvfrom(s, msg, datasize, 0, &aSocketAddress, &aLength);
		if(n < datasize)	
		{
			perror("server recvfrom");
			printf("returned %d\n", n);
			return;
		}
/*printf("received %d\n", loop);*/
		n = sendto(s, (char *)&reply, sizeof(reply), 0, &aSocketAddress,
			sizeof(struct sockaddr_in));
		if(n < sizeof(reply)) 
		{
			perror("server sendto");
			printf("returned %d\n", n);
			return;
		}
	}
	close(s);
}


/**
 * Act as a client, repeatedly sending UDP messages of given size to machine at given port
 */
void client(char *machine, int port, int datasize)
{
	int s, n, reply, aLength;
	register int loop;
	struct sockaddr_in mySocketAddress, yourSocketAddress, aSocketAddress;

	if((msg = (char *)malloc(datasize)) == 0)	
	{
		perror("client: not enough memory!");
		return;
	}

    if(( s = socket(AF_INET, SOCK_DGRAM, 0))<0) 
        {
		perror("client: socket failed");
		return;
	}

	makeLocalSA(&mySocketAddress);
    if( bind(s, &mySocketAddress, sizeof(struct sockaddr_in))!= 0)
        {
		perror("client: Bind failed\n");
		close (s);
		return;
	}

	printSA(mySocketAddress);
	makeDestSA(&yourSocketAddress, machine, port);
	/*printSA(yourSocketAddress);*/
	aLength = sizeof(aSocketAddress);
	for(loop = 0; loop < NTIMES; ++loop)	
	{
		n = sendto(s, (char *)&msg, datasize, 0, &yourSocketAddress,
			sizeof(struct sockaddr_in));
		if(n < datasize) 
		{
			perror("client sendto");
			printf("returned %d\n", n);
			return;
		}
		n = recvfrom(s, (char *)&reply, sizeof(reply), 0, 
				&aSocketAddress, &aLength);
		if(n < sizeof(reply))	
		{
			perror("client recvfrom");
			printf("returned %d\n", n);
			return;
		}
	}
	close(s);
}

long time_diff(struct timeval *tp2, struct timeval *tp1)
{
        long    secs, microsecs;

        secs = (tp2->tv_sec - tp1->tv_sec);
        microsecs = (tp2->tv_usec - tp1->tv_usec);
        return secs * 1000000 + microsecs ;
}