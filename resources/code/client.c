/*
 Coded by Hyunjae Lee and Jihun Lim
 
 This is a simple implementation for latency evaluation in a V2V network.
 We assume that all node knows other MAC addresses who want to communicate with. 

 [Client]
 : Client works as a sender, it sends a message regurally to a specific node with MAC address which The client already know.

  *Reference : https://stackoverflow.com/questions/10824827/raw-sockets-communication-over-wifi-receiver-not-able-to-receive-packets
*/
#include <pthread.h>
#include "result_structure.h"
#include "write_data.h"
#include "get_nic_index.h"
#include "packet_size.h"
#include "control_client.h"
#include "control_server.h"
#include "get_configuration.h"

void *sock_recv_thread();

int main(int argc, char *argv[])
{
    /*
	- LOCAL VARIABLE

	s_dest_addr : save the value DEST_ADDR and protocols
	s32_sock : save the socket file descriptor
	s32_res : save the value that check the socket's status
	u16_data_off : save the length of frame header; in this case, the length will be 14
	u16_i : save the index for the message's count
	pu8a_frame : save the address of frame
	pu8a_data : save the address of payload

	curTime : save the current time for analyzing
	*/

    struct timespec client_send;

    uint16_t u16_i = 0;
    pthread_t ul_recv_thd_id = -1;
    /*
	get_mac_addr() : fill the SRC_MAC and DEST_MAC value to gu8a_src_mac, gu8a_dest_mac array
	get_nice_name() : fill the NIC_NAME value to NIC_NAME
	*/

    initArray();

    if (get_configuration() != NO_ERROR) {
        return 0;
    }

    if (openTextFile(argv[1]) != NO_ERROR) {
        return 0;
    }

    if (set_packet_size(argv[2]) != NO_ERROR) {
        return 0;
    }

    /*
	this part is creating frame and socket for sending the message
	there are some variable to save the value of frame and socket

	pu8a_frame and pu8a_data variables are linked to each address of frame and payload

	pu8a_frame                      pu8a_data
	v                               v
	 ==============================================================
	| src_mac | dest_mac | protocol |       payload                |
	 ==============================================================

	
	*/

    if(init_client_frame(get_packet_size(FRAME)) != NO_ERROR) {
        return 0;
    }

    set_client_frame_header(get_src_addr(), get_dest_addr());

    init_client_socket();
    init_client_sockaddr_ll();

    if (set_client_socket() != NO_ERROR) {
        free_client_frame();
        return 0;
    }

    set_client_sockaddr_ll(get_nic_index(get_nic_name()), get_dest_addr());

    fflush(stdout);
    (void)pthread_create(&ul_recv_thd_id, NULL, sock_recv_thread, NULL);
    sleep(1);

    /*
	this part is saving the values for target socket
	
	when the receiver will receive the message, 
	the receiver will check this message is for his machine or not using this infomation
	*/

    /*
	this part is preparing the message and sending the message to target
	we put the timestamp value for analyzing the latency
	if the receiver will get this message, the receiver will analyze the latency using this information

	and this function will be executed every 1 second

	pu8a_frame                      pu8a_data
	v                               v
	 ==============================================================
	| src_mac | dest_mac | protocol | "string + index + timestamp" |
	 ==============================================================

	*/

    printf("******Sending data using raw socket over  %s \n", get_nic_name());

    while (1)
    {
        init_client_data(get_packet_size(DATA));

        clock_gettime(CLOCK_MONOTONIC_RAW, &client_send);
        putSendTime(client_send.tv_nsec, u16_i);

        set_client_data(get_packet_size(DATA), u16_i++, client_send.tv_nsec);

        printf("Client sent a message %d\n", u16_i - 1);

        if (send_client_data(get_packet_size(FRAME)) != NO_ERROR) {
            free_client_frame();
            close_client_socket();

            return 0;
        }

        sleep(1);
    }

    return 0;
}

void * sock_recv_thread() {
    struct timespec client_recv;

    if (init_server_frame(get_packet_size(FRAME)) != NO_ERROR) {
        return 0;
    }
    
    init_server_socket();
    init_server_sockaddr_ll();

    if (set_server_socket() != NO_ERROR) {
        free_server_frame();
        return 0;
    }

    printf("Socket Receiver created\n");

    if (bind_server_socket() != NO_ERROR) {
        free_server_frame();
        close_server_socket();
        return 0;
    }

    printf("Socket bind successful\n");

    while (1)
    {
        init_server_sockaddr_ll();

        if (receive_data(get_packet_size(FRAME)) != NO_ERROR) {
            free_server_frame();
            close_server_socket();

            return 0;
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &client_recv);

        if (check_data_from_target(get_dest_addr()) == FROM_TARGET) {
            print_target_mac_addr();
            parse_data();

            putRecvTime(client_recv.tv_nsec, atoi(get_packet_index()));
            putDiff(get_packet_timestamp(), get_packet_index());

            printData(get_packet_index());

            if (isFull() == FULL) {
                for (int index = 0; index < MAXIMUM - 1; index++) {
                    writeDataToText(getNetworkLatency(index));
                }

                closeTextFile();
                free_server_frame();
                close_server_socket();

                return 0;
            }
        }
    }
}