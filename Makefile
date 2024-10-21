all: dispatcher client_exec

dispatcher: dis.o server.o
	gcc -g -Wall dis.o server.o -o dispatcher -pthread -lrt

client_exec: client_exec.o
	gcc -g -Wall client_exec.o -o client_exec -pthread -lrt

dis.o: dis.c
	gcc -g -Wall -c dis.c

server.o: server.c
	gcc -g -Wall -c server.c

client_exec.o: client_exec.c
	gcc -g -Wall -c client_exec.c

clean:
	rm -f dispatcher client_exec *.o