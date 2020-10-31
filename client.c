#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>


int main(){
	printf("\nClient Process - Started...");

	char *req_GET = "GET / HTTP/1.0\r\n\r\n";				       //the GET request to the server
	char buffer[3000]; 		         		       		       //buffer, temp. space for data
	FILE *output_file;

	//TODO: Socket Creation
	//int socket(int domain, int type, int protocol) creates an endpoint for communication ~ man pages
	//domain: AF_INET6  -- selects IPv6
	//type: SOCK_STREAM  -- TCP socket
	//protocol: 0 -- only one DEFAULT protocol per type, can be replaced with 0
	int ipv6_socket = socket(AF_INET6, SOCK_STREAM, 0);

	if(ipv6_socket == -1) {    //in case of error
		perror("\n--Error: Socket not created--- LINE 19\n");
		return -1;
	} else printf("\nSocket created succesfully...");

	
	//TODO: get the address and hostname, using the Networking API
	//struct addrinfo will hold this information
	//getaddrinfo() converts human-readable strings into dynamically allocated linked lists of struct addrinfo structures (see wiki)
	//further documentation can be found in the Networking API docs (wiki)
	
	struct addrinfo hints, *result, *pointer;
	memset(&hints, 0, sizeof(hints));    //allocate space in the memory block
	hints.ai_family = AF_INET6;	     //IPv6
	hints.ai_socktype = SOCK_STREAM;     //TCP
							
	int address_info = getaddrinfo("www.axu.tm", "80", &hints, &result);    //domain, HTTP port, return space, returned results (linked list)
	
	if(address_info != 0) {
		perror("\n--- Error: getaddrinfo()--- LINE 36\n");
		return -1;
	}
	
	//TODO: Traverse the linked list and check for socket validity using ai_next, which pints toward next struct
										
	for(pointer = result; pointer!=NULL; pointer = pointer -> ai_next) {
		//validity check:
		int temp_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol); 
		if(temp_socket == -1) {
			perror("\n--- ERROR: Socket is not valid --- LINE 48");
			continue;	//ignore if not valid
		}
	

		//Connect to desired address
	
		if(connect(ipv6_socket, pointer->ai_addr, pointer->ai_addrlen) == -1) { 
			//this means that connectivity failed
			shutdown(ipv6_socket, 2);   //close the socket, 0-stop Rx, 1 - stop Rx and ack, 2 - stop both Tx and Rx
		
			perror("\n--- ERROR: Connection error --- LINE 56");
			continue;	//ignore if we cannot connect
		}
		
		break;
	}

	if ( pointer == NULL ) {
		perror("\n--- ERROR: Connection failed --- LINE 67");
		return -1; 
	}	

	printf("\nConnection succsesful.\n");
	

	//TODO: After succesfully connected to server, try to send the GET request.
	
	int server_reply = send(ipv6_socket, req_GET, strlen(req_GET), 0); //0 - sets flags in 0;
	if (server_reply == -1) {
		perror("\n--- ERROR: GET Request FAILED --- LINE 77");
	} else printf("\nGET request sent to server");

	memset(buffer, 0, 3000);  //allocate space for buffer

	//TODO: Receive data from server
	
	while(1) {  //infinte loop
		int received_reply = recv(ipv6_socket, buffer, 3000-1,0);		//returns message length if succesful, -1 error, 0 terminated
		if(received_reply == -1){
			perror("\n--- ERROR: No info received, breaking --- LINE 87");
			break;		//cannot continue, break.
		} else if(received_reply == 0) {
			printf("\nConnection ended, closing...\n");
			break;
		} 
		printf("\nConnected, fetching...\n");
		buffer[received_reply] = '\0';		//appends at the end the string termiantion

		//TODO: Write to file
		output_file = fopen("index.html", "w");
		fprintf(output_file, buffer);		//write to file, overwrites if exists, creates if doesn't exist
	}
	return 0;
}
