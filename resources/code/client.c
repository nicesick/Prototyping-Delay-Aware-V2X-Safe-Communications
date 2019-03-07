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
    struct timespec client_send;

    uint16_t u16_i = 0;
    pthread_t ul_recv_thd_id = -1;



/*
Initiate other modules : result_structure.c , get_configuration.c , write_data.c , packet_size.c
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
Initiate client socket
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



/*
Create pthread for receiving the message
*/
    fflush(stdout);
    (void)pthread_create(&ul_recv_thd_id, NULL, sock_recv_thread, NULL);
    sleep(1);

    printf("******Sending data using raw socket over  %s \n", get_nic_name());



    while (1)
    {
/*
Sending the message to the server
*/
        init_client_data(get_packet_size(DATA));

        clock_gettime(CLOCK_MONOTONIC_RAW, &client_send);
        putSendTime(client_send.tv_nsec, u16_i);

        set_client_data(get_packet_size(DATA), "raw_packet_test", \
        u16_i++, \
        client_send.tv_nsec);

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


/*
Initiate server socket
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



    while (1)
    {
/*
Receive the message from target
*/
        init_server_sockaddr_ll();

        if (receive_data(get_packet_size(FRAME)) != NO_ERROR) {
            free_server_frame();
            close_server_socket();

            return 0;
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &client_recv);

        if (check_data_from_target(get_dest_addr()) == FROM_TARGET) {
            parse_data();

            if (check_correct_data("Index/Diff") == CORRECT_DATA) {
                printf("==============================================\n");
                print_target_mac_addr();

                print_packet_string();
                print_packet_index();
                print_packet_timestamp();
                
                putRecvTime(client_recv.tv_nsec, atoi(get_packet_index()));
                putDiff(atol(get_packet_timestamp()), atoi(get_packet_index()));

                printData(atoi(get_packet_index()));



/*
If the array that save the latency is full, the contents are saved to text file
*/
                if (isFull() == FULL) {
                    for (int index = 0; index < MAXIMUM - 1; index++) {
                        writeDataToText(getNetworkLatency(index));
                    }

                    closeTextFile();
                    free_server_frame();
                    close_server_socket();

                    return 0;
                }

                printf("==============================================\n");
            }
        }
    }
}