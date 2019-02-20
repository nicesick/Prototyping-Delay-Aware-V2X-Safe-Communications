setup: setup.o get_nic_index.o
	gcc -o setup setup.o get_nic_index.o

server: server.o mac.o get_nic_index.o circular_array.o
	gcc -o server server.o mac.o get_nic_index.o circular_array.o

client: client.o mac.o get_nic_index.o circular_array.o
	gcc -pthread -o client client.o mac.o get_nic_index.o circular_array.o

setup.o: setup.c setup.h get_nic_index.h
	gcc -c -o setup.o setup.c
	
server.o: mac.h get_nic_index.h circular_array.h server.c
	gcc -c -o server.o server.c

client.o: mac.h get_nic_index.h circular_array.h client.c
	gcc -pthread -c -o client.o client.c

circular_array.o: circular_array.h circular_array.c
	gcc -c -o circular_array.o circular_array.c

mac.o: mac.h mac.c
	gcc -c -o mac.o mac.c

get_nic_index.o: get_nic_index.h get_nic_index.c
	gcc -c -o get_nic_index.o get_nic_index.c