/* example code from : timeserv.c */
/* A simple UDP server that sends the current date and time to the client */
/* Last modified: September 20, 2005 */
/* http://www.gomorgan89.com */
/* Link with library file wsock32.lib */
/* multithreading from https://msdn.microsoft.com/en-us/library/windows/desktop/ms682516(v=vs.85).aspx */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include <unistd.h>
#include <windows.h>
#include <stdio.h>
#include "mesinkar.c"

#define MAX_SEM_COUNT 2
#define THREADCOUNT 2
#define DELAY 500
#define BUFFER_SIZE 40

/** **/
#include "buffer.c"
Tab buf;
int ind = 1;

//#include "buffer.h"
/** **/

	WSADATA w;							/* Used to open windows connection */
	unsigned short port_number;			/* Port number to use */
	SOCKET sd;							/* Socket descriptor of server */

	int client_length;					/* Length of client struct */
	struct sockaddr_in client;			/* Information about the client */

	char msg;							/* Current char msg*/
	int nByte =0;

	char send_buffer[BUFFER_SIZE] = "XON\r\n";		/* Data to send */
	char send_bufferXOFF[BUFFER_SIZE] = "XOFF\r\n";
	char buffer[BUFFER_SIZE];			/* Where to store received data */
	struct hostent *hp;						/* Information about the server */

	int server_length;						/* Length of server struct */
	struct sockaddr_in server;				/* Information about the server */

	int bytes_received;					/* Bytes received from client */

	int a1, a2, a3, a4;						/* Server address components in xxx.xxx.xxx.xxx form */
	int b1, b2, b3, b4;						/* Client address components in xxx.xxx.xxx.xxx form */
	char host_name[256];					/* Host name of this computer */

	int isOn = 0;
	int i;

	HANDLE ghSemaphore;
	HANDLE aThread[THREADCOUNT];
    DWORD ThreadID;

void usage(void);


void SendXON(){
	
	printf("Send XON...\n");
	/* Tranmsit signal XON */
	client_length = sizeof(struct sockaddr_in);
	if (sendto(sd, send_buffer, (int)strlen(send_buffer) + 1, 0, (struct sockaddr *)&client, client_length) == -1)
	{
		fprintf(stderr, "Error transmitting data.\n");
		closesocket(sd);
		WSACleanup();
		exit(0);
	}
}

void SendXOFF(){
	
	printf("Send XOFF...\n");
	/* Tranmsit signal XOFF */
	client_length = sizeof(struct sockaddr_in);
	if (sendto(sd, send_bufferXOFF, (int)strlen(send_bufferXOFF) + 1, 0, (struct sockaddr *)&client, client_length) == -1)
	{
		fprintf(stderr, "Error transmitting data.\n");
		closesocket(sd);
		WSACleanup();
		exit(0);
	}
}


DWORD WINAPI ProcRcvData( LPVOID lpParam)
{

	/* Receive data */
	while(1){
		client_length = (int)sizeof(struct sockaddr_in);

		/* Receive bytes from client */
		bytes_received = recvfrom(sd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client, &client_length);
		if (bytes_received < 0)
		{
			fprintf(stderr, "Could not receive datagram.\n");
			closesocket(sd);
			WSACleanup();
			exit(0);
		} 
//		printf("%s\n", buffer);
		if ( (strcmp(buffer, "START\r\n") == 0) )
		{
			if (recvfrom(sd, (char *)&msg, (int)sizeof(msg), 0, (struct sockaddr *)&client, &client_length) < 0)
			{
				fprintf(stderr, "Error receiving data.\n");
				//closesocket(sd);
				//WSACleanup();
				//exit(0);
			} else {
				nByte++;
				/* Show message */
				printf("Menerima byte ke - %d.\n", nByte);
				Add(&buf, msg); // masuk buffer

				//printf("Mengonsumsi ke - %d : %c\n", nByte,msg);
			}
		}


	}

	closesocket(sd);
	WSACleanup();

	return 0;
}

DWORD WINAPI ProcWaitSignal( LPVOID lpParam){
	while(1){
		printf("checking queue\n");
        Sleep(1000);
        if (NbElmt != 0){
        	if ((!IsUpper(buf)) && (!IsLower(buf))) {
	            printf("Mengonsumsi ke - %d : %c\n", ind, buf.TI[ind]);
	            ind++;
	            NbElmt--; // konsumsi buffer            
	        } else if (IsUpper(buf)) {
	           	SendXOFF();
	           	printf("Masuk Upper limit\n");
	            printf("Mengonsumsi ke - %d : %c\n", ind, buf.TI[ind]);
	            ind++;
	            NbElmt--; // konsumsi buffer
	        } else if (IsLower(buf)) {
	            SendXON();
	            printf("Masuk Lower limit\n");
	            // no consume0
	        }
        }
        
	}
	return 0;
};

int main (int argc, char **argv)
{

	/*** CHECK ARGUMEN VALIDATION ********/
	/* Interpret command line */
	if (argc == 2)
	{
		/* Use local address */
		if (sscanf(argv[1], "%u", &port_number) != 1)
		{
			usage();
		}
	}
	else if (argc == 3)
	{
		/* Copy address */
		if (sscanf(argv[1], "%d.%d.%d.%d", &a1, &a2, &a3, &a4) != 4)
		{
			usage();
		}
		if (sscanf(argv[2], "%u", &port_number) != 1)
		{
			usage();
		}
	}
	else
	{
		usage();
	}

	/******END ARGUMEN VALIDATION **************/
	/* Open windows connection */
	if (WSAStartup(0x0101, &w) != 0)
	{
		fprintf(stderr, "Could not open Windows connection.\n");
		exit(0);
	}

	/* Open a datagram socket */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd == INVALID_SOCKET)
	{
		fprintf(stderr, "Could not create socket.\n");
		WSACleanup();
		exit(0);
	}

	/* Clear out server struct */
	memset((void *)&server, '\0', sizeof(struct sockaddr_in));

	/* Set family and port */
	server.sin_family = AF_INET;
	server.sin_port = htons(port_number);

	/* Set address automatically if desired */
	if (argc == 2)
	{
		/* Get host name of this computer */
		gethostname(host_name, sizeof(host_name));
		hp = gethostbyname(host_name);

		/* Check for NULL pointer */
		if (hp == NULL)
		{
			fprintf(stderr, "Could not get host name.\n");
			closesocket(sd);
			WSACleanup();
			exit(0);
		}
		
		/* Assign the address */
		server.sin_addr.S_un.S_un_b.s_b1 = hp->h_addr_list[0][0];
		server.sin_addr.S_un.S_un_b.s_b2 = hp->h_addr_list[0][1];
		server.sin_addr.S_un.S_un_b.s_b3 = hp->h_addr_list[0][2];
		server.sin_addr.S_un.S_un_b.s_b4 = hp->h_addr_list[0][3];
	}
	/* Otherwise assign it manually */
	else
	{
		server.sin_addr.S_un.S_un_b.s_b1 = (unsigned char)a1;
		server.sin_addr.S_un.S_un_b.s_b2 = (unsigned char)a2;
		server.sin_addr.S_un.S_un_b.s_b3 = (unsigned char)a3;
		server.sin_addr.S_un.S_un_b.s_b4 = (unsigned char)a4;
	}

	/* Bind address to socket */
	if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1)
	{
		fprintf(stderr, "Could not bind name to socket.\n");
		closesocket(sd);
		WSACleanup();
		exit(0);
	}

	/* Print out server information */
	printf("Binding pada %u.%u.%u.%u : %u\n", (unsigned char)server.sin_addr.S_un.S_un_b.s_b1,
											  (unsigned char)server.sin_addr.S_un.S_un_b.s_b2,
											  (unsigned char)server.sin_addr.S_un.S_un_b.s_b3,
											  (unsigned char)server.sin_addr.S_un.S_un_b.s_b4,
											  (unsigned short) port_number);
	printf("Press CTRL + C to quit\n");

	/* Loop and get data from clients */
	// Create a semaphore with initial and max counts of MAX_SEM_COUNT

    ghSemaphore = CreateSemaphore(
        NULL,           // default security attributes
        MAX_SEM_COUNT,  // initial count
        MAX_SEM_COUNT,  // maximum count
        NULL);          // unnamed semaphore

    if (ghSemaphore == NULL)
    {
        printf("CreateSemaphore error: %d\n", GetLastError());
        return 1;
    }

    // Create 2 worker threads aThread[0] for reading and sending file
    aThread[0] = CreateThread(
                 NULL,       // default security attributes
                 0,          // default stack size
                 (LPTHREAD_START_ROUTINE) ProcRcvData,
                 NULL,       // no thread function arguments
                 0,          // default creation flags
                 &ThreadID); // receive thread identifier
    //case of making thread error
    if( aThread[0] == NULL )
    {
        printf("CreateThread error: %d\n", GetLastError());
        return 1;
    }
    //aThread[1] for listening XON and XOFF
    aThread[1] = CreateThread(
                 NULL,       // default security attributes
                 0,          // default stack size
                 (LPTHREAD_START_ROUTINE) ProcWaitSignal,
                 NULL,       // no thread function arguments
                 0,          // default creation flags
                 &ThreadID); // receive thread identifier
    //case of making thread error
    if( aThread[1] == NULL )
    {
        printf("CreateThread error: %d\n", GetLastError());
        return 1;
    }

    // Wait for all threads to terminate

    WaitForMultipleObjects(THREADCOUNT, aThread, TRUE, INFINITE);

    // Close thread and semaphore handles

    for( i=0; i < THREADCOUNT; i++ )
        CloseHandle(aThread[i]);

    CloseHandle(ghSemaphore);
    return 0;
}

void usage(void)
{
	fprintf(stderr, "receiver [optional input server_address] port_num\n");
	exit(0);
}
