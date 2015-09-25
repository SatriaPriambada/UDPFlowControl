/* timecli.c */
/* Gets the current time from a UDP server */
/* Last modified: September 23, 2005 */
/* http://www.gomorgan89.com */
/* Link with library file wsock32.lib */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include <windows.h>
#include <stdio.h>
#include "mesinkar.c"
#define MAX_SEM_COUNT 2
#define THREADCOUNT 2
#define DELAY 500
#define BUFFER_SIZE 40

	WSADATA w;								/* Used to open Windows connection */
	unsigned short port_number;				/* The port number to use */
	SOCKET sd;								/* The socket descriptor */

	int client_length;						/* Length of client struct */
	struct sockaddr_in client;				/* Information about the client */

	char msg;								/* Char from file */
	int nByte =0;
	char* filename;							//string of file name

	char send_buffer[BUFFER_SIZE]="START\r\n";		/* Data to send */
	char buffer[BUFFER_SIZE];			/* Where to store received data */
	struct hostent *hp;						/* Information about the server */

	int server_length;						/* Length of server struct */
	struct sockaddr_in server;				/* Information about the server */

	int bytes_received;					/* Bytes received from client */

	int a1, a2, a3, a4;						/* Server address components in xxx.xxx.xxx.xxx form */
	int b1, b2, b3, b4;						/* Client address components in xxx.xxx.xxx.xxx form */
	char host_name[256];					/* Host name of this computer */

	int isOn = 1;
	int i;

	HANDLE ghSemaphore;
	HANDLE aThread[THREADCOUNT];
    DWORD ThreadID;



void usage(void);

DWORD WINAPI ProcSendData( LPVOID lpParam){
	
	while (1)
	{
		client_length = (int)sizeof(struct sockaddr_in);
		nByte =0;

		/* Get current char */
		START(filename);
	    while (!EOP)
	    {
	    	if (isOn == 1){
		    	nByte++;
		        msg = CC;
		        /* Tranmsit data to send char file */
				server_length = sizeof(struct sockaddr_in);
				if (sendto(sd, send_buffer, (int)strlen(send_buffer) + 1, 0, (struct sockaddr *)&server, server_length) == -1)
				{
					fprintf(stderr, "Error transmitting data.\n");
					closesocket(sd);
					WSACleanup();
					exit(0);
				}
		        /* Send data back */
				if (sendto(sd, &msg, (int)sizeof(msg), 0, (struct sockaddr *)&server, server_length) != (int)sizeof(msg))
				{
					fprintf(stderr, "Error sending datagram.\n");
					closesocket(sd);
					WSACleanup();
					exit(0);
				} else {
					printf("Mengirim byte ke - %d : ' %c '\n",nByte,msg);
				}
				
		        ADV();
		        printf("\n");
		    } //else do nothing
		    Sleep(DELAY);
	    }
	} 
	closesocket(sd);
	WSACleanup();

	return 0;
}

DWORD WINAPI ProcWaitSignal( LPVOID lpParam){
	while(1){
		printf("thread 2\n");
        /* Receive bytes from client */
        bytes_received = recvfrom(sd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server, &server_length);
        if (bytes_received < 0)
        {
            fprintf(stderr, "No signal received.\n");
            //closesocket(sd);
            //WSACleanup();
            //exit(0);
        }
        printf("String buffer :%s\n", buffer);
        
        if (strcmp(buffer, "XON\r\n") == 0) {
            isOn = 1;
            printf("XON Diterima Melanjutkan transmitting...\n");
        } else if (strcmp(buffer, "XOFF\r\n") == 0) {
            isOn = 0;
            printf("Menunggu XON...\n");
            
        }
        //Sleep(1000);
        
	}
	return 0;
};

int main(int argc, char **argv)
{
	/***CHECK ARGUMEN VALIDATON*******/
	if ( argc != 4)
	{
		usage();
	}
	if (sscanf(argv[1], "%d.%d.%d.%d", &a1, &a2, &a3, &a4) != 4)
	{
		usage();
	}
	if (sscanf(argv[2], "%u", &port_number) != 1)
	{
		usage();
	}
/*	if (argc == 4)
	{
		if (sscanf(argv[3], "%d.%d.%d.%d", &b1, &b2, &b3, &b4) != 4)
		{
			usage();
		}
	}*/
	/***END ARGUMEN VALIDATON*******/

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

	/* Set server address */
	server.sin_addr.S_un.S_un_b.s_b1 = (unsigned char)a1;
	server.sin_addr.S_un.S_un_b.s_b2 = (unsigned char)a2;
	server.sin_addr.S_un.S_un_b.s_b3 = (unsigned char)a3;
	server.sin_addr.S_un.S_un_b.s_b4 = (unsigned char)a4;

	/* Clear out client struct */
	memset((void *)&client, '\0', sizeof(struct sockaddr_in));

	/* Set family and port */
	client.sin_family = AF_INET;
	client.sin_port = htons(0);

	if (argc == 3)
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
		client.sin_addr.S_un.S_un_b.s_b1 = hp->h_addr_list[0][0];
		client.sin_addr.S_un.S_un_b.s_b2 = hp->h_addr_list[0][1];
		client.sin_addr.S_un.S_un_b.s_b3 = hp->h_addr_list[0][2];
		client.sin_addr.S_un.S_un_b.s_b4 = hp->h_addr_list[0][3];
	}
	else
	{
		client.sin_addr.S_un.S_un_b.s_b1 = (unsigned char)b1;
		client.sin_addr.S_un.S_un_b.s_b2 = (unsigned char)b2;
		client.sin_addr.S_un.S_un_b.s_b3 = (unsigned char)b3;
		client.sin_addr.S_un.S_un_b.s_b4 = (unsigned char)b4;
	}

	/* Bind local address to socket */
	if (bind(sd, (struct sockaddr *)&client, sizeof(struct sockaddr_in)) == -1)
	{
		fprintf(stderr, "Cannot bind address to socket.\n");
		closesocket(sd);
		WSACleanup();
		exit(0);
	}
	/****************************END TRANSMIT DATA**************************/
    filename = argv[3];
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
                 (LPTHREAD_START_ROUTINE) ProcSendData, 
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
	fprintf(stderr, "Usage: transmitter server_address port [optional input client_address]\n");
	exit(0);
}