all: transmitter receiver
transmitter: TransSpek.c mesinkar.c
	gcc -o transmitter TransSpek.c -lws2_32 -I.
receiver: RecSpek.c buffer.c
	gcc -o receiver RecSpek.c -lws2_32 -I.