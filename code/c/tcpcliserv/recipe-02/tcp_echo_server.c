/* tcp_echo_server.c - main, tcp_echo_server */

#define	_USE_BSD
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "error_handling.h"
#include "tcp_passive.h"

#define	QLEN		  32	/* maximum connection queue length	*/
#define	BUFSIZE		4096

void reaper(int);
int	tcp_echo_server(int fd);

/*------------------------------------------------------------------------
 * main - Concurrent TCP server for ECHO service
 *------------------------------------------------------------------------
 */
int main(int argc, char *argv[])
{
	char	*service = "echo";	/* service name or port number	*/
	struct	sockaddr_in fsin;	/* the address of a client	*/
	unsigned int	alen;		/* length of client's address	*/
	int	msock;			/* master server socket		*/
	int	ssock;			/* slave server socket		*/

	switch (argc) {
	case	1:
		break;
	case	2:
		service = argv[1];
		break;
	default:
		error_handling("usage: tcp_echo_server [port]\n");
	}

	msock = tcp_passive(service, QLEN);

	(void) signal(SIGCHLD, reaper);

	while (1) {
		alen = sizeof(fsin);
		ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
		if (ssock < 0) {
			if (errno == EINTR)
				continue;
			error_handling("accept: %s\n", strerror(errno));
		}
		switch (fork()) {
		case 0:		/* child */
			(void) close(msock);
			exit(tcp_echo_server(ssock));
		default:	/* parent */
			(void) close(ssock);
			break;
		case -1:
			error_handling("fork: %s\n", strerror(errno));
		}
	}
}

/*------------------------------------------------------------------------
 * tcp_echo_server - echo data until end of file
 *------------------------------------------------------------------------
 */
int
tcp_echo_server(int fd)
{
	char	buf[BUFSIZ];
	int	cc;

	while (cc = read(fd, buf, sizeof buf)) {
		if (cc < 0)
			error_handling("echo read: %s\n", strerror(errno));
		if (write(fd, buf, cc) < 0)
			error_handling("echo write: %s\n", strerror(errno));
	}
	return 0;
}

/*------------------------------------------------------------------------
 * reaper - clean up zombie children
 *------------------------------------------------------------------------
 */
void reaper(int sig)
{
	int	status;

	while (wait3(&status, WNOHANG, (struct rusage *)0) >= 0)
		/* empty */;
}
