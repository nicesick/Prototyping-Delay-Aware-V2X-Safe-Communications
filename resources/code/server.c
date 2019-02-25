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

#include "main_header.h"
#include "result_structure.h"
#include "mac.h"
#include "get_nic_index.h"

/*
- EXTERN VARIABLE

NIC_NAME : save the value NIC_NAME using the function "get_nic_name()" from get_nic_index.h
gu8a_src_mac : save the value SRC_MAC using the function "get_mac_addr()" from mac.h
gu8a_dest_mac : save the value DEST_MAC using the function "get_mac_addr()" from mac.h
*/

extern char *NIC_NAME;
extern uint8_t gu8a_src_mac[6];
extern uint8_t gu8a_dest_mac[6];

int main(void)
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

    struct sockaddr_ll s_src_addr;
    int32_t s32_sock = -1;
    int32_t s32_res = -1;
    uint16_t u16_data_off = 0;
    uint8_t *pu8a_frame = NULL;
    uint16_t u16_i = 0;
    struct timespec server_send, server_recv;
    long temp;
    double avg = 0;
    char *sArr[3] = {
        NULL,
    };
    int flag = 0;

    /*
	get_mac_addr() : fill the SRC_MAC and DEST_MAC value to gu8a_src_mac, gu8a_dest_mac array
	get_nic_name() : fill the NIC_NAME value to NIC_NAME
	*/

    get_mac_addr();
    get_nic_name();

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

    u16_data_off = (uint16_t)(ETH_FRAME_LEN - ETH_DATA_LEN);
    pu8a_frame = (uint8_t *)calloc(ETH_FRAME_LEN, 1);
    s32_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if (-1 == s32_sock)
    {
        perror("Could not create the socket");
        goto LABEL_CLEAN_EXIT;
    }
    printf("Server) Socket created\n");

    if (NULL == pu8a_frame)
    {
        printf("Could not get memory for the receive frame\n");
        goto LABEL_CLEAN_EXIT;
    }

    (void)memset(&s_src_addr, 0, sizeof(s_src_addr));

    /*
	this part is saving the values for target socket

	when the receiver will receive the message,
	the receiver will check this message is for his machine or not using this infomation
	*/

    s_src_addr.sll_family = AF_PACKET;
    /*we don't use a protocol above ethernet layer, just use anything here*/
    s_src_addr.sll_protocol = htons(ETH_P_ALL);
    s_src_addr.sll_ifindex = get_nic_index((char *)NIC_NAME);
    s_src_addr.sll_hatype = ARPHRD_ETHER;
    s_src_addr.sll_pkttype = PACKET_HOST; //PACKET_OTHERHOST;
    s_src_addr.sll_halen = ETH_ALEN;

    s_src_addr.sll_addr[0] = gu8a_src_mac[0];
    s_src_addr.sll_addr[1] = gu8a_src_mac[1];
    s_src_addr.sll_addr[2] = gu8a_src_mac[2];
    s_src_addr.sll_addr[3] = gu8a_src_mac[3];
    s_src_addr.sll_addr[4] = gu8a_src_mac[4];
    s_src_addr.sll_addr[5] = gu8a_src_mac[5];

    s32_res = bind(s32_sock,
                   (struct sockaddr *)&s_src_addr,
                   sizeof(s_src_addr));

    if (-1 == s32_res)
    {
        perror("Could not bind to the socket");
        goto LABEL_CLEAN_EXIT;
    }

    printf("Socket bind successful\n");

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
        clock_gettime(CLOCK_MONOTONIC_RAW, &server_recv);

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
            printf("==============================================\n");
            printf("Received data from ");

            for (u16_i = 0; u16_i < sizeof(s_sender_addr.sll_addr) - 2; u16_i++)
            {
                printf("%02x:", s_sender_addr.sll_addr[u16_i]);
            }

            printf("\n");
            char *str = strtok(&pu8a_frame[u16_data_off], " ");
            u16_i = 0;

            flag = 0;
            for (u16_i = 0; u16_i < sizeof(s_sender_addr.sll_addr) - 2; u16_i++)
            {
                if (s_sender_addr.sll_addr[u16_i] != gu8a_src_mac[u16_i])
                {
                    flag = 1;
                    break;
                }
            }

            if (flag == 1)
            {
                printf("Server received data from others : %s\n\n", &pu8a_frame[u16_data_off]);
            }
            else
            {
                u16_i = 0;
                while (str != NULL)
                {
                    sArr[u16_i] = str;
                    u16_i++;

                    str = strtok(NULL, " ");
                }

                printf("  Received data from client : ");

                for (u16_i = 0; u16_i < 3; u16_i++)
                {
                    if (u16_i == 0)
                    {
                        printf("%s ", sArr[u16_i]);
                    }
                    else if (u16_i == 1)
                    {
                        printf("%s ", sArr[u16_i]);
                    }

                    else if (u16_i == 2)
                    {
                        temp = atol(sArr[u16_i]);

                        if (temp <= 0)
                        {
                            printf("Unexpected packet\n");
                            break;
                        }

                        printf("msg sent at %ld (ns)\n", temp);

                        (void)memcpy(pu8a_frame, gu8a_src_mac, ETH_ALEN);
                        (void)memcpy(pu8a_frame + ETH_ALEN, gu8a_dest_mac, ETH_ALEN);

                        clock_gettime(CLOCK_MONOTONIC_RAW, &server_send);

                        (void)snprintf((char *)&pu8a_frame[u16_data_off],
                                       ETH_DATA_LEN,
                                       "Index/Diff %d %ld", atoi(sArr[u16_i - 1]), timespec_diff(server_send.tv_nsec, server_recv.tv_nsec));

                        s32_res = sendto(s32_sock,
                                         pu8a_frame,
                                         ETH_FRAME_LEN,
                                         0,
                                         (struct sockaddr *)&s_src_addr,
                                         sizeof(s_src_addr));

                        if (-1 == s32_res)
                        {
                            perror("Socket send failed");
                            goto LABEL_CLEAN_EXIT;
                        }
                    }
                }
            }
        }
    }

LABEL_CLEAN_EXIT:
    return EXIT_SUCCESS;
}