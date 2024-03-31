main.o : server.o error.o http_header.o http_message.o util.o socket.o
	gcc -o main.o error.o server.o http_header.o http_message.o util.o socket.o

error.o : error.c
	gcc -c error.c

server.o : server.c
	gcc -c server.c

http_header.o : http_header.c
	gcc -c http_header.c

http_message.o : http_message.c
	gcc -c http_message.c

util.o : util.c
	gcc -c util.c

socket.o : socket.c
	gcc -c socket.c