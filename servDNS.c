/* servDNS.c */

#include <stdlib.h>
#include <stdio.h>
#include <cnaiapi.h>
#include <assert.h>

#define BUFFSIZE		256
#define CACHESIZE		5

/* for boolean type */

typedef int bool;
#define true 1
#define false 0

/*-----------------------------------------------------------------------
 *
 * Program: servDNS
 * Purpose: wait for a connection from a client DNS and echo the address given in IP form
 * Usage:   servDNS <appnum>
 *
 *-----------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	connection	conn;
	int		len, i, j, new_len; //new_len is the length of the received message's length plus the word in the end which indicates where the address has been found
	char		buff[BUFFSIZE];
	char		cacheString[CACHESIZE][BUFFSIZE]; //string array for the host-names of the addresses
	computer	cacheIP[CACHESIZE]; //computer array for the IP of the addresses (in decimal) (parallel with cacheString[][])
	computer	comp;
	bool		foundLocally = 0; //boolean type -> changes value to 1 when the address given has been found locally inside the cache
	int			counter = 0; //counter which stores the cache position for the next address

	unsigned char octet[4]  = {0,0,0,0}; // to save the final form of the IP with dots !
	
	/* fill arrays with dummy data */
	
	strcpy(cacheString[0], "www.google.com\n");
	cacheIP[0] = 605542828; // <- address in decimal. Actual IP address : 172.217.23.36
	strcpy(cacheString[1], "www.facebook.com\n");
	cacheIP[1] = 587329693; // <- address in decimal. Actual IP address : 157.240.1.35
	strcpy(cacheString[2], "compus.uom.gr\n");
	cacheIP[2] = 3923114947; // <- address in decimal. Actual IP address : 195.251.213.233
	strcpy(cacheString[3], "www.uom.gr.com\n");
	cacheIP[3] = 3856006083; // <- address in decimal. Actual IP address : 195.251.213.229
	strcpy(cacheString[4], "www.eyath.gr\n");
	cacheIP[4] = 90762637; // <- address in decimal. Actual IP address : 141.237.104.5

	
	
	if (argc != 2) {
		(void) fprintf(stderr, "usage: %s <appnum>\n", argv[0]);
		exit(1);
	}

	/* wait for a connection from a client DNS */

	conn = await_contact((appnum) atoi(argv[1]));
	if (conn < 0)
		exit(1);

	memset(buff,0,BUFFSIZE); //initialize buff
	
	/* iterate, echoing the IP of the address received until end of file */

	while((len = recv(conn, buff, BUFFSIZE, 0)) > 0){
		
		if (strcmp("\n",buff) == 0) sprintf(buff, "-1");
		
		/* Check the cache */
		
		for(i=0; i<CACHESIZE; i++){
			
			if(strcmp(cacheString[i],buff) == 0){ //if the message which came from the Client is equal to the string of the cache in the 'i' position
				
				/*Convert each byte from all 4 bytes of the address to char and then print them as integers inside the buff with dots between*/
				
				for (j=0; j<4; j++){
					octet[j] = ( cacheIP[i] >> (j*8) ) & 0xFF;
				}
				
				sprintf(buff, "%d.%d.%d.%d LOCAL", octet[0],octet[1],octet[2],octet[3]); //assign string to the buff
				
				foundLocally = 1;
				break;
			}
		}
		
		/* Check non-LOCALLY */
		
		if(!foundLocally){ //if the address has not found locally
			
			buff[strlen(buff)-1] = '\0';
			
			comp = cname_to_comp(buff); // assign comp(variable) with the decimal address that matches the address stored inside the buff
			
			if (comp == -1 ){ //if the address has not found on-line (with the function cname_to_comp)
				
				sprintf(buff, "-1 UKNOWN"); //assign string to the buff
				
			}
			
			else{ //if cname_to_comp has returned a valid address , convert it from decimal to IP form
				
				/*replace the address of the cache with the new non-LOCAL address and update the counter which indicates the next position*/
				
				cacheIP[counter] = comp;
				sprintf(cacheString[counter],"%s\n",buff);
				counter ++;
				if (counter == CACHESIZE) counter = 0; //if the last element of the array is reached initialize the counter to 0
				
				/*Convert each byte from all 4 bytes of the address to char and then print them as integers inside the buff with dots between*/
				
				for (j=0; j<4; j++){ 
					octet[j] = ( comp >> (j*8) ) & 0xFF;
				}
				
				sprintf(buff, "%d.%d.%d.%d non-LOCAL", octet[0],octet[1],octet[2],octet[3]); //assign string to the buff
			}
			
		}
		
		new_len = strlen(buff); //define the new length of the message we are going to echo on the screen
		
		(void) send(conn, buff, new_len, 0); //send the buff(string) to the Client in order to echo it on the screen
		
		/* Initialize buff (fill with 0) and also initialize foundLocally (boolean value) with 0 */
	
		memset(buff,0,BUFFSIZE); //initialize buff
		foundLocally = 0;
			
	}
	send_eof(conn);
	return 0;
}
