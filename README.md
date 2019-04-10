DNS Server in C with cache implementation

Instructions to run:

- Run servDNS from command prompt: servDNS (default 20000).
- Run clDNS from command prompt: clDNS <Server's IP> (default 20000).
- From clDNS enter host names and server will return its IP address. If host name does not exist, returns -1.
- To exit, CTRL+C or just ENTER.

>>Libraries used: Douglas Comer's cnaiapi.<<

clDNS.c :   Once connected to the server, it reads an address from the user,
	          sends this address to the server through the send function. Subsequently
	          receives the corresponding IP address from the server via the recv function
	          and displays it on the screen with the write function.

servDNS.c : After making a connection with the client, it receives a string address,
	    searches in the local cache (2 parallel arrays cacheString[][] and
	    cacheIP[]).

	   A) If found *converts the address into the appropriate format,
	   stores it in the buff variable and sends it to the client.

	   B) If the address is not located locally, the function
	   cname_to_comp (cnaiapi.h) is executed which returns (if it exists) the
	   IP address. Again buff is sent to the client
	   (after *conversion). (If IP is found in this step, the cache 
	   is getting informed by entering this new address at the accordingly next position).

	   C) Finally, if the address is invalid
	   the value (-1) is returned from cname_to_comp function
	   and the corresponding message is entered into the buff again
	   to be sent to the client.

*Conversion: The original IP format in all cases is decimal.
	      For this reason, one by one, all bytes are converted to char
	      to then enter the buff in the right format with the right
 	      separators (dots) between the bytes.
