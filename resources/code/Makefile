setup: setup.o get_nic_index.o
	gcc -o setup setup.o get_nic_index.o

server: server.o mac.o get_nic_index.o result_structure.o packet_size.o
	gcc -o server server.o mac.o get_nic_index.o result_structure.o packet_size.o

client: client.o mac.o get_nic_index.o result_structure.o write_data.o packet_size.o
	gcc -pthread -o client client.o mac.o get_nic_index.o result_structure.o write_data.o packet_size.o

setup.o: setup.c setup.h common_header.h get_nic_index.h
	gcc -c -o setup.o setup.c
	
server.o: main_header.h mac.h get_nic_index.h result_structure.h packet_size.h server.c
	gcc -c -o server.o server.c

client.o: main_header.h mac.h get_nic_index.h result_structure.h write_data.h packet_size.h client.c
	gcc -pthread -c -o client.o client.c

packet_size.o: common_header.h packet_size.h packet_size.c
	gcc -c -o packet_size.o packet_size.c

write_data.o: write_data.h write_data.c
	gcc -c -o write_data.o write_data.c

result_structure.o: common_header.h result_structure.h result_structure.c
	gcc -c -o result_structure.o result_structure.c

mac.o: common_header.h mac.h mac.c
	gcc -c -o mac.o mac.c

get_nic_index.o: common_header.h get_nic_index.h get_nic_index.c
	gcc -c -o get_nic_index.o get_nic_index.c
