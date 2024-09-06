all: project3 client_exec

project3: project3.o server.o dis.o
	gcc -g -Wall project3.o server.o dis.o -o project3 -pthread -lrt

client_exec: client_exec.o
	gcc -g -Wall client_exec.o -o client_exec -pthread -lrt

project3.o: project3.c
	gcc -g -Wall -c project3.c

client_exec.o: client_exec.c
	gcc -g -Wall -c client_exec.c

server.o: server.c
	gcc -g -Wall -c server.c

dis.o: dis.c
	gcc -g -Wall -c dis.c

clean:
	rm -f project3 project3.o server.o dis.o client_exec client_exec.o