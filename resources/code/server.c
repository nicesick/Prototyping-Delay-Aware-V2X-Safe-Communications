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
    struct timespec server_send, server_recv;


/*
Initiate other modules : get_configuration.c , packet_size.c
*/
    if (get_configuration() != NO_ERROR) {
        return 0;
    }

    if (set_packet_size(argv[1]) != NO_ERROR) {
        return 0;
    }



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



/*
Initiate client socket
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
/*
Receive the message from the target
*/
        init_server_sockaddr_ll();

        if (receive_data(get_packet_size(FRAME)) != NO_ERROR) {
            free_server_frame();
            close_server_socket();
            free_client_frame();
            close_client_socket();
            return 0;
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &server_recv);

        if (check_data_from_target(get_src_addr()) == FROM_TARGET) {
            parse_data();

            if (check_correct_data("raw_packet_test") == CORRECT_DATA) {
                printf("==============================================\n");
                print_target_mac_addr();

                print_packet_string();
                print_packet_index();
                print_packet_timestamp();


/*
Send the message back to the target
*/
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

                printf("==============================================\n");
            }
        }
    }

    return 0;
}