server: server.o mac.o get_nic_index.o
	gcc -o server server.o mac.o get_nic_index.o

client: client.o mac.o get_nic_index.o
	gcc -o client client.o mac.o get_nic_index.o

server.o: time_micro.h mac.h get_nic_index.h server.c
	gcc -c -o server.o server.c

client.o: time_micro.h mac.h get_nic_index.h client.c
	gcc -c -o client.o client.c

mac.o: mac.h mac.c
	gcc -c -o mac.o mac.c

get_nic_index.o: get_nic_index.h get_nic_index.c
	gcc -c -o get_nic_index.o get_nic_index.c