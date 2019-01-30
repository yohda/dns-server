server: dns_server.o rbtree.o hash.o
	gcc -o server dns_server.o rbtree.o hash.o

deg_server: dns_server.o rbtree.o hash.o
	gcc -g -o server dns_server.o rbtree.o hash.o  

dns_server.o: dns_server.c
	gcc -c dns_server.c

rbtree.o: rbtree.c
	gcc -c rbtree.c

hash.o: hash.c
	gcc -c hash.c

all: server

deg: deg_server
