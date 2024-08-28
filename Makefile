all: project3

project3: project3.o server.o dis.o clients.o
	gcc -g -Wall project3.o server.o dis.o clients.o -o project3 -pthread -lrt

project3.o: project3.c
	gcc -g -Wall -c project3.c

server.o: server.c
	gcc -g -Wall -c server.c

dis.o: dis.c
	gcc -g -Wall -c dis.c

clients.o: clients.c
	gcc -g -Wall -c clients.c

clean:
	rm -f project3 project3.o server.o dis.o clients.o