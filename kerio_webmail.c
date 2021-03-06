#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>
#include <sys/signal.h>
#include <arpa/nameser.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

#define TIMEOUT 3

#define TCP_NODELAY 0

char buf[8192];
char buf2[8192];

const char dacrap[] =
/*"\x50\x4F\x53\x54\x20\x2F\x64\x65\x66\x61\x75\x6C\x74\x2F\x64\x6F"
"\x6C\x6F\x67\x69\x6E\x2E\x70\x68\x70\x20\x48\x54\x54\x50\x2F\x31"
"\x2E\x30\x0D\x0A\x48\x6F\x73\x74\x3A\x20\x31\x30\x2E\x31\x30\x2E"
"\x31\x30\x2E\x31\x39\x0D\x0A\x41\x63\x63\x65\x70\x74\x3A\x20\x74"
"\x65\x78\x74\x2F\x68\x74\x6D\x6C\x2C\x20\x74\x65\x78\x74\x2F\x70"
"\x6C\x61\x69\x6E\x2C\x20\x74\x65\x78\x74\x2F\x73\x67\x6D\x6C\x2C"
"\x20\x2A\x2F\x2A\x3B\x71\x3D\x30\x2E\x30\x31\x0D\x0A\x41\x63\x63"
"\x65\x70\x74\x2D\x45\x6E\x63\x6F\x64\x69\x6E\x67\x3A\x20\x67\x7A"
"\x69\x70\x2C\x20\x63\x6F\x6D\x70\x72\x65\x73\x73\x0D\x0A\x41\x63"
"\x63\x65\x70\x74\x2D\x4C\x61\x6E\x67\x75\x61\x67\x65\x3A\x20\x65"
"\x6E\x0D\x0A\x50\x72\x61\x67\x6D\x61\x3A\x20\x6E\x6F\x2D\x63\x61"
"\x63\x68\x65\x0D\x0A\x43\x61\x63\x68\x65\x2D\x43\x6F\x6E\x74\x72"
"\x6F\x6C\x3A\x20\x6E\x6F\x2D\x63\x61\x63\x68\x65\x0D\x0A\x55\x73"
"\x65\x72\x2D\x41\x67\x65\x6E\x74\x3A\x20\x4C\x79\x6E\x78\x2F\x32"
"\x2E\x38\x2E\x34\x72\x65\x6C\x2E\x31\x20\x6C\x69\x62\x77\x77\x77"
"\x2D\x46\x4D\x2F\x32\x2E\x31\x34\x0D\x0A\x52\x65\x66\x65\x72\x65"
"\x72\x3A\x20\x68\x74\x74\x70\x3A\x2F\x2F\x31\x30\x2E\x31\x30\x2E"
"\x31\x30\x2E\x31\x39\x2F\x64\x65\x66\x61\x75\x6C\x74\x2F\x6C\x6F"
"\x67\x69\x6E\x2E\x70\x68\x70\x0D\x0A\x43\x6F\x6F\x6B\x69\x65\x32"
"\x3A\x20\x24\x56\x65\x72\x73\x69\x6F\x6E\x3D\x22\x31\x22\x0D\x0A"
"\x43\x6F\x6F\x6B\x69\x65\x3A\x20\x50\x48\x50\x53\x45\x53\x53\x49"
"\x44\x3D\x66\x35\x62\x31\x38\x62\x34\x34\x62\x38\x64\x33\x34\x66"
"\x66\x30\x62\x66\x39\x61\x66\x38\x34\x38\x63\x30\x32\x64\x33\x61"
"\x36\x37\x0D\x0A\x43\x6F\x6E\x74\x65\x6E\x74\x2D\x74\x79\x70\x65"
"\x3A\x20\x61\x70\x70\x6C\x69\x63\x61\x74\x69\x6F\x6E\x2F\x78\x2D"
"\x77\x77\x77\x2D\x66\x6F\x72\x6D\x2D\x75\x72\x6C\x65\x6E\x63\x6F"
"\x64\x65\x64\x0D\x0A\x43\x6F\x6E\x74\x65\x6E\x74\x2D\x6C\x65\x6E"
"\x67\x74\x68\x3A\x20\x33\x33\x0D\x0A\x0D\x0A\x6B\x67\x6A\x64\x61"
"\x3D\x41\x64\x6D\x69\x6E\x26\x6B\x67\x6B\x30\x30\x3D\x6D\x69\x73"
"\x74\x65\x72\x64\x69\x6E\x67\x61\x6C\x69\x6E\x67"; */
"\x47\x45\x54\x20\x2F\x64\x65\x66\x61\x75\x6C\x74\x2F\x6C\x6F\x67"
"\x69\x6E\x2E\x70\x68\x70\x3F\x72\x65\x61\x73\x6F\x6E\x3D\x66\x61"
"\x69\x6C\x75\x72\x65\x20\x48\x54\x54\x50\x2F\x31\x2E\x30\x0D\x0A"
"\x48\x6F\x73\x74\x3A\x20\x31\x30\x2E\x31\x30\x2E\x31\x30\x2E\x31"
"\x39\x0D\x0A\x41\x63\x63\x65\x70\x74\x3A\x20\x74\x65\x78\x74\x2F"
"\x68\x74\x6D\x6C\x2C\x20\x74\x65\x78\x74\x2F\x70\x6C\x61\x69\x6E"
"\x2C\x20\x74\x65\x78\x74\x2F\x73\x67\x6D\x6C\x2C\x20\x2A\x2F\x2A"
"\x3B\x71\x3D\x30\x2E\x30\x31\x0D\x0A\x41\x63\x63\x65\x70\x74\x2D"
"\x45\x6E\x63\x6F\x64\x69\x6E\x67\x3A\x20\x67\x7A\x69\x70\x2C\x20"
"\x63\x6F\x6D\x70\x72\x65\x73\x73\x0D\x0A\x41\x63\x63\x65\x70\x74"
"\x2D\x4C\x61\x6E\x67\x75\x61\x67\x65\x3A\x20\x65\x6E\x0D\x0A\x55"
"\x73\x65\x72\x2D\x41\x67\x65\x6E\x74\x3A\x20\x4C\x79\x6E\x78\x2F"
"\x32\x2E\x38\x2E\x34\x72\x65\x6C\x2E\x31\x20\x6C\x69\x62\x77\x77"
"\x77\x2D\x46\x4D\x2F\x32\x2E\x31\x34\x0D\x0A\x52\x65\x66\x65\x72"
"\x65\x72\x3A\x20\x68\x74\x74\x70\x3A\x2F\x2F\x31\x30\x2E\x31\x30"
"\x2E\x31\x30\x2E\x31\x39\x2F\x64\x65\x66\x61\x75\x6C\x74\x2F\x6C"
"\x6F\x67\x69\x6E\x2E\x70\x68\x70\x0D\x0A\x43\x6F\x6F\x6B\x69\x65"
"\x32\x3A\x20\x24\x56\x65\x72\x73\x69\x6F\x6E\x3D\x22\x31\x22\x0D"
"\x0A\x43\x6F\x6F\x6B\x69\x65\x3A\x20\x50\x48\x50\x53\x45\x53\x53"
"\x49\x44\x3D\x66\x35\x62\x31\x38\x62\x34\x34\x62\x38\x64\x33\x34"
"\x66\x66\x30\x62\x66\x39\x61\x66\x38\x34\x38\x63\x30\x32\x64\x33"
"\x61\x36\x37\x0D\x0A\x0D\x0A";  

#define CRAPLEN (sizeof(dacrap)-1)



int send_crap() {
	memcpy(buf, dacrap, CRAPLEN);
	return CRAPLEN;
}



void corruptor(char *buf, int len) {
int cb, i, l;

	cb = rand()%15+1; /* bytes to corrupt */

	for (i=0; i < cb; i++)
	{
		l = rand()%len;
		buf[l] = rand()%256;
	}
}




void diffit() {
int i;
        printf("DIFF:\n");
        for (i=0; i < CRAPLEN; i++)
        {
                if (buf[i] != dacrap[i])
                        printf("Offset %d: 0x%x -> 0x%x\n", i, dacrap[i] & 0x000000FF, buf[i] & 0x000000FF);
        }
        printf("*****\n");
}






int main(int argc, char *argv[]) {
	struct sockaddr_in addr;
	int s, port = 0, len, counter, totalcounter, mu, last, seeder, x;
	char *host = NULL;
	unsigned int seed;
	struct timeval tv;

	printf("Generic Protocol fuzzer [propz to syzop]\n\n");

        if (argc < 3) {
                fprintf(stderr, "Use: %s [ip] [port]\n", argv[0]);
                exit(1);
        }
	

	host = argv[1];
	port = atoi(argv[2]);
	if ((port < 1) || (port > 65535)) {
		fprintf(stderr, "Port out of range (%d)\n", port);
		exit(1);
	}

        memset(&tv, 0, sizeof(tv));
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;


	memset(&addr, 0, sizeof(addr));


	signal(SIGPIPE, SIG_IGN); /* Ignore SIGPIPE */
        counter = 0;
        fprintf(stdout, "Fuzzing...\n");
        while(1) {
            counter++;
	    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Socket error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	    }
            setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));        //recv timeout
	    setsockopt(s,IPPROTO_TCP,TCP_NODELAY,&x,sizeof(x));         //disable nagles alg.

	    addr.sin_family = AF_INET;
	    addr.sin_port = htons(port);
	    addr.sin_addr.s_addr = inet_addr(host);
	    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                sleep(1);
                if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		    fprintf(stdout, "Unable to connect: %s\n", strerror(errno));
	            diffit();
                    exit(0);
                }
	    }

	    len = send_crap();
	    corruptor(buf, len);         /* puts 1-17 random bytes into buf */
	    write(s, buf, len);
	    write(s,buf, rand() % len);   //new stuff
            memset (&buf2, 0, sizeof(buf2));
            mu=recv(s,buf2,sizeof(buf2),0);
            if ( (mu != last) && (counter > 1) ) {
                    fprintf(stdout, "return buffer from scanned host just changed Counter=%d\n", counter);
                    fprintf(stdout, "expected %d return bytes...received %d bytes\n", last, mu);
                    diffit();
            }
            last = mu;
            totalcounter = 0;
	    close(s);
        }
	
	exit(EXIT_SUCCESS);
}

