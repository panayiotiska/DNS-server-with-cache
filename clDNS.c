/* clDNS.c */

#include <stdlib.h>
#include <stdio.h>
#include <cnaiapi.h>
#include <string.h>

#define BUFFSIZE		256
#define INPUT_PROMPT		"Input   > "
#define RECEIVED_PROMPT		"Received> "

int readln(char *, int);

/*-----------------------------------------------------------------------
 *
 * Program: clDNS
 * Purpose: contact servDNS, send user input and print server response
 * Usage:   clDNS <compname> [appnum]
 * Note:    Appnum is optional. If not specified the standard echo appnum
 *          (7) is used.
 *
 *-----------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	computer	comp;
	appnum		app;
	connection	conn;
	char		buff[BUFFSIZE];
	int		expect, received, len, new_len; //new len is the length of the new received message!

	if (argc < 2 || argc > 3) {
		(void) fprintf(stderr, "usage: %s <compname> [appnum]\n",
			       argv[0]);
		exit(1);
	}

	/* convert the arguments to binary format comp and appnum */

	comp = cname_to_comp(argv[1]);
	if (comp == -1)
		exit(1);

	if (argc == 3)
		app = (appnum) atoi(argv[2]);
	else
		if ((app = appname_to_appnum("echo")) == -1)
			exit(1);
	
	/* form a connection with the servDNS */

	conn = make_contact(comp, app);
	if (conn < 0) 
		exit(1);

	(void) printf(INPUT_PROMPT);
	(void) fflush(stdout);

	/* iterate: read input from the user, send to the server,	*/
	/*	    receive reply from the server, and display for user */

	while((len = readln(buff, BUFFSIZE)) > 0) {

		/* send the input to the servDNS */

		(void) send(conn, buff, len, 0);
		(void) printf(RECEIVED_PROMPT);
		(void) fflush(stdout);

		/* read and print the new no. of bytes from server DNS */

		new_len = recv(conn, buff, BUFFSIZE, 0);
		   
		(void) write(STDOUT_FILENO, buff, new_len);
		
		(void) printf("\n");
		(void) printf(INPUT_PROMPT);
		(void) fflush(stdout);
	}

	/* iteration ends when EOF found on stdin */

	(void) send_eof(conn);
	(void) printf("\n");
	return 0;
}
