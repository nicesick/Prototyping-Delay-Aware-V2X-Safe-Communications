/*
 Coded by Hyunjae Lee and Jihun Lim
 
 This is a simple implementation for latency evaluation in a V2V network.
 We assume that all node knows other MAC addresses who want to communicate with

 [Server]
 This is a receiver that is waiting until it gets a message with link-layer header from the client.
 Whenever the receiver gets a message from clients,
 the receiver shows user link-layer header and payload.

  *Reference : https://stackoverflow.com/questions/10824827/raw-sockets-com$
*/

#include "result_structure.h"
#include "get_nic_index.h"
#include "get_configuration.h"
#include "packet_size.h"
#include "control_server.h"
#include "control_client.h"

int main(int argc, char *argv[])
{
    /*
	- LOCAL VARIABLE

	s_src_addr : save the value SRC_ADDR and protocols
	s32_sock : save the socket file descriptor
	s32_res : save the value that check the socket's status
	u16_data_off : save the length of frame header; in this case, the length will be 14
	pu8a_frame : save the address of frame
	u16_i : save the index for the message's count

	lastTime : save the current time for analyzing
	temp : save the time from the sender
	avg : save the average latency values

	sArr : parse and save the each values from the message
	sArr[0] : save raw_socket_test string
	sArr[1] : save the index of the message
	sArr[2] : save the sending time of the message

	flag :  if the message is from the client we are executing, the flag will be 0
			otherwise, the flag will be 1

		case the flag == 1:
			just printing the message
		case the flag == 0:
			parse and save the each values to sArr array
	*/

    struct timespec server_send, server_recv;

    /*
	get_mac_addr() : fill the SRC_MAC and DEST_MAC value to gu8a_src_mac, gu8a_dest_mac array
	get_nic_name() : fill the NIC_NAME value to NIC_NAME
	*/

    if (get_configuration() != NO_ERROR) {
        return 0;
    }

    if (set_packet_size(argv[1]) != NO_ERROR) {
        return 0;
    }
    /*
	this part is creating frame and socket for receiving the message
	there are some variable to save the value of frame and socket

	pu8a_frame variables is linked to address of frame
	u16_data_off save the length value of the frame header

	pu8a_frame                      pu8a_frame[u16_data_off]
	v ----------------------------- v                               
	==============================================================
	| src_mac | dest_mac | protocol |       payload                |
	==============================================================


	*/

    if (init_server_frame(get_packet_size(FRAME)) != NO_ERROR) {
        return 0;
    }

    init_server_socket();
    init_server_sockaddr_ll();
    set_server_sockaddr_ll(get_nic_index(get_nic_name()));

    if (set_server_socket() != NO_ERROR) {
        free_server_frame();
        return 0;
    }

    if (bind_server_socket() != NO_ERROR) {
        free_server_frame();
        close_server_socket();
        return 0;
    }

    /*
	this part is preparing the message and receiving the message from target
	we get the timestamp value for analyzing the latency
	if the receiver will get this message, the receiver will analyze the latency using this information

	pu8a_frame                      
	v                               
	==============================================================
	| src_mac | dest_mac | protocol | "string + index + timestamp" |
	==============================================================

	there are some step for checking the message
	
	for example, we can assume there are two MAC address
	target sender's MAC address : ff:ff:ff:ff:ff:ff
	the MAC address we received : 00:00:00:00:00:00

	we just check those MAC address is same or not

	if the mac address is same each other, the flag will be 0
	otherwise, the flag will be 1

	if the flag is 0, we just print the message
	if the flag is 1, we have to parse and save the each values to sArr array

	the each values will be saved in each sArr container
	sArr[0] = raw_socket_test
	sArr[1] = the index of the message
	sArr[2] = the sending time of the message

	finally we can calculate the latency between currentTime and the sending time
	the latency's result will be currentTime - the sending time
	
	the average variable adds the latency's value
	and will be devided by sArr[1] value
	*/

    if(init_client_frame(get_packet_size(FRAME)) != NO_ERROR) {
        free_server_frame();
        close_server_socket();
        return 0;
    }

    set_client_frame_header(get_dest_addr(), get_src_addr());

    init_client_socket();
    init_client_sockaddr_ll();

    if (set_client_socket() != NO_ERROR) {
        free_server_frame();
        close_server_socket();
        free_client_frame();
        return 0;
    }

    set_client_sockaddr_ll(get_nic_index(get_nic_name()), get_src_addr());

    while (1)
    {
        init_server_sockaddr_ll();

        if (receive_data(get_packet_size(FRAME)) != NO_ERROR) {
            free_server_frame();
            close_server_socket();
            free_client_frame();
            close_client_socket();
            return 0;
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &server_recv);
        
        printf("==============================================\n");
        print_target_mac_addr();

        if (check_data_from_target(get_src_addr()) == FROM_TARGET) {
            parse_data();

            if (check_correct_data("raw_packet_test") != CORRECT_DATA) {
                continue;
            }

            print_packet_string();
            print_packet_index();
            print_packet_timestamp();

            init_client_data(get_packet_size(DATA));

            clock_gettime(CLOCK_MONOTONIC_RAW, &server_send);

            set_client_data(get_packet_size(DATA), "Index/Diff", \
            atoi(get_packet_index()), \
            timespec_diff(server_recv.tv_nsec, server_send.tv_nsec));

            if (send_client_data(get_packet_size(FRAME)) != NO_ERROR) {
                free_server_frame();
                close_server_socket();
                free_client_frame();
                close_client_socket();

                return 0;
            }
        }

        printf("==============================================\n");
    }

    return 0;
}