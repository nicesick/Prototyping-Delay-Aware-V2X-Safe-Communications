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

void *
sock_recv_thread()
{
    struct sockaddr_ll s_src_addr;
    int32_t s32_sock = -1;
    int32_t s32_res = -1;
    uint16_t u16_data_off = 0;
    uint8_t *pu8a_frame = NULL;
    uint16_t u16_i = 0;
    int flag;
    char *sArr[3] = {
        NULL,
    };

    struct timespec client_recv;
    long diff;

    printf("Socket receive thread\n");

    u16_data_off = (uint16_t)(ETH_FRAME_LEN - ETH_DATA_LEN);

    pu8a_frame = (uint8_t *)calloc(ETH_FRAME_LEN, 1);

    s32_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if (-1 == s32_sock)
    {
        perror("Could not create the socket");
        goto LABEL_CLEAN_EXIT;
    }

    printf("Receiver) Socket created\n");
    if (NULL == pu8a_frame)
    {
        printf("Could not get memory for the receive frame\n");
        goto LABEL_CLEAN_EXIT;
    }

    (void)memset(&s_src_addr, 0, sizeof(s_src_addr));

    s_src_addr.sll_family = AF_PACKET;
    /*we don't use a protocol above ethernet layer, just use anything here*/
    s_src_addr.sll_protocol = htons(ETH_P_ALL);
    s_src_addr.sll_ifindex = get_nic_index((char *)NIC_NAME);
    s_src_addr.sll_hatype = ARPHRD_ETHER;
    s_src_addr.sll_pkttype = PACKET_HOST; //PACKET_OTHERHOST;
    s_src_addr.sll_halen = ETH_ALEN;

    s32_res = bind(s32_sock,
                   (struct sockaddr *)&s_src_addr,
                   sizeof(s_src_addr));

    if (-1 == s32_res)
    {
        perror("Could not bind to the socket");
        goto LABEL_CLEAN_EXIT;
    }

    printf("Socket bind successful\n");

    while (1)
    {
        struct sockaddr_ll s_sender_addr;
        socklen_t u32_sender_addr_len = sizeof(s_sender_addr);

        (void)memset(&s_sender_addr, 0, sizeof(s_sender_addr));

        s32_res = recvfrom(s32_sock,
                           pu8a_frame,
                           ETH_FRAME_LEN,
                           0,
                           (struct sockaddr *)&s_sender_addr,
                           &u32_sender_addr_len);

        clock_gettime(CLOCK_MONOTONIC_RAW, &client_recv);

        if (-1 == s32_res)
        {
            perror("Socket receive failed");
            break;
        }
        else if (s32_res < 0)
        {
            perror("Socket receive, error ");
        }
        else
        {
            u16_i = 0;

            char *str = strtok(&pu8a_frame[u16_data_off], " ");

            flag = 0;
            for (u16_i = 0; u16_i < sizeof(s_sender_addr.sll_addr) - 2; u16_i++)
            {
                if (s_sender_addr.sll_addr[u16_i] != gu8a_dest_mac[u16_i])
                {
                    flag = 1;
                    break;
                }
            }

            if (flag == 0)
            {

                printf("  Received data from server : ");

                for (u16_i = 0; u16_i < sizeof(s_sender_addr.sll_addr) - 2; u16_i++)
                {
                    printf("%02x:", s_sender_addr.sll_addr[u16_i]);
                }
                printf("\n");

                u16_i = 0;
                while (str != NULL)
                {
                    sArr[u16_i] = str;
                    u16_i++;

                    str = strtok(NULL, " ");
                }

                for (u16_i = 0; u16_i < 3; u16_i++)
                {
                    if (u16_i == 0)
                    {
                        printf("%s ", sArr[u16_i]);
                    }
                    else if (u16_i == 1)
                    {
                        printf("%s ", sArr[u16_i]);
                        putRecvTime(client_recv.tv_nsec, atoi(sArr[u16_i]));
                    }
                    else if (u16_i == 2)
                    {
                        diff = atol(sArr[u16_i]);
                        putDiff(diff, atoi(sArr[u16_i - 1]));

                        printf("\n MESSAGE[%d] LATENCY : %ld\n", atoi(sArr[1]), getMessageLatency(atoi(sArr[u16_i - 1])));
                        printf(" NETWORK[%d] LATENCY : %ld\n", atoi(sArr[1]), getNetworkLatency(atoi(sArr[u16_i - 1])));
                        printf(" DIFF : %ld\n\n", diff);

                        if (isFull() == FULL)
                        {
                            for (int k = 0; k < MAXIMUM - 1; k++)
                            {
                                writeDataToText(getNetworkLatency(k));
                            }

                            closeTextFile();
                            initArray();
                        }
                    }
                }
            }
        }
    }

LABEL_CLEAN_EXIT:
    return (NULL);
}