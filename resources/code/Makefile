all: setup client server

setup: get_configuration.o setup.o
	gcc -o setup setup.o get_configuration.o

server: server.o get_nic_index.o result_structure.o packet_size.o
	gcc -o server server.o get_nic_index.o result_structure.o packet_size.o

client: client.o get_nic_index.o result_structure.o write_data.o packet_size.o control_client.o get_configuration.o
	gcc -pthread -o client client.o get_nic_index.o result_structure.o write_data.o packet_size.o control_client.o get_configuration.o

setup.o: setup.h common_header.h get_configuration.h setup.c
	gcc -c -o setup.o setup.c
	
server.o: main_header.h mac.h get_nic_index.h result_structure.h packet_size.h server.c
	gcc -c -o server.o server.c

client.o: get_nic_index.h result_structure.h write_data.h packet_size.h get_configuration.h control_client.h client.c
	gcc -pthread -c -o client.o client.c

control_server.o: main_header.h common_header.h control_server.h control_server.c
	gcc -c -o 

control_client.o: main_header.h common_header.h control_client.h control_client.c
	gcc -c -o control_client.o control_client.c

packet_size.o: common_header.h packet_size.h packet_size.c
	gcc -c -o packet_size.o packet_size.c

write_data.o: write_data.h write_data.c
	gcc -c -o write_data.o write_data.c

result_structure.o: common_header.h result_structure.h result_structure.c
	gcc -c -o result_structure.o result_structure.c

get_configuration.o: common_header.h get_configuration.h get_configuration.c
	gcc -c -o get_configuration.o get_configuration.c

get_nic_index.o: common_header.h get_nic_index.h get_nic_index.c
	gcc -c -o get_nic_index.o get_nic_index.c

clean:
	rm -f *.o setup client server
