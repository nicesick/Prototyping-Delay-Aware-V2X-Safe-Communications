/*
 Coded by Hyunjae Lee and Jihun Lim
 
 This is a simple implementation for latency evaluation in a V2V network.
 We assume that all node knows other MAC addresses who want to communicate with. 

 [Client]
 : Client works as a sender, it sends a message regurally to a specific node with MAC address which The client already know.

  *Reference : https://stackoverflow.com/questions/10824827/raw-sockets-communication-over-wifi-receiver-not-able-to-receive-packets
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <linux/if.h>
#include <linux/if_arp.h>
#include <arpa/inet.h>
#include <time.h>
#include "mac.h" /* MAC address */
#include "get_nic_index.h"

/*
- EXTERN VARIABLE

NIC_NAME : save the value NIC_NAME using the function "get_nic_name()" from get_nic_index.h
gu8a_src_mac : save the value SRC_MAC using the function "get_mac_addr()" from mac.h
gu8a_dest_mac : save the value DEST_MAC using the function "get_mac_addr()" from mac.h
*/

extern char* NIC_NAME;
extern uint8_t gu8a_src_mac[6];
extern uint8_t gu8a_dest_mac[6];

int main(void)
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

    struct sockaddr_ll s_dest_addr;
    int32_t s32_sock = -1;
    int32_t s32_res = -1;
    uint16_t u16_data_off = 0;
    uint16_t u16_i = 0;
    uint8_t *pu8a_frame = NULL;
    uint8_t *pu8a_data = NULL;
    //long curTime;
    struct timespec curTime;
	/*
	get_mac_addr() : fill the SRC_MAC and DEST_MAC value to gu8a_src_mac, gu8a_dest_mac array
	get_nice_name() : fill the NIC_NAME value to NIC_NAME
	*/
    get_mac_addr();
    get_nic_name();

    printf("Socket raw test\n");

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
    (void)memset(&s_dest_addr, 0, sizeof(s_dest_addr));
    pu8a_frame = (uint8_t *)calloc(ETH_FRAME_LEN, 1);

    if (NULL == pu8a_frame)
    {
        printf("Could not get memory for the transmit frame\n");
        goto LABEL_CLEAN_EXIT;
    }

    u16_data_off = (uint16_t)(ETH_FRAME_LEN - ETH_DATA_LEN);
    pu8a_data = pu8a_frame + u16_data_off;
    s32_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if (-1 == s32_sock)
    {
        perror("Could not create the socket");
        goto LABEL_CLEAN_EXIT;
    }

    printf("Client) Socket created\n");
    fflush(stdout);

    sleep(1);

	/*
	this part is saving the values for target socket
	
	when the receiver will receive the message, 
	the receiver will check this message is for his machine or not using this infomation
	*/

    s_dest_addr.sll_family = AF_PACKET;
    /*we don't use a protocol above ethernet layer, just use anything here*/
    s_dest_addr.sll_protocol = htons(ETH_P_ALL);
    s_dest_addr.sll_ifindex = get_nic_index((uint8_t *)NIC_NAME);
    s_dest_addr.sll_hatype = ARPHRD_ETHER;
    s_dest_addr.sll_pkttype = PACKET_OTHERHOST; //PACKET_OUTGOING
    s_dest_addr.sll_halen = ETH_ALEN;
    /*MAC - begin*/
    s_dest_addr.sll_addr[0] = gu8a_dest_mac[0];
    s_dest_addr.sll_addr[1] = gu8a_dest_mac[1];
    s_dest_addr.sll_addr[2] = gu8a_dest_mac[2];
    s_dest_addr.sll_addr[3] = gu8a_dest_mac[3];
    s_dest_addr.sll_addr[4] = gu8a_dest_mac[4];
    s_dest_addr.sll_addr[5] = gu8a_dest_mac[5];
    /*MAC - end*/
    s_dest_addr.sll_addr[6] = 0x00; /*not used*/
    s_dest_addr.sll_addr[7] = 0x00; /*not used*/

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

    (void)memcpy(pu8a_frame, gu8a_dest_mac, ETH_ALEN);
    (void)memcpy(pu8a_frame + ETH_ALEN, gu8a_src_mac, ETH_ALEN);

    printf("******Sending data using raw socket over  %s \n", NIC_NAME);

    while (1)
    {
        (void)memset(&pu8a_frame[u16_data_off], '\0', ETH_DATA_LEN);

        clock_gettime(CLOCK_MONOTONIC_RAW, &curTime);

        (void)snprintf((char *)&pu8a_frame[u16_data_off],
                       ETH_DATA_LEN,
                       "raw_packet_test %d %ld", u16_i++, curTime.tv_nsec);

        printf("Client sent a message %d\n", u16_i - 1);

        s32_res = sendto(s32_sock,
                         pu8a_frame,
                         ETH_FRAME_LEN,
                         0,
                         (struct sockaddr *)&s_dest_addr,
                         sizeof(s_dest_addr));

        if (-1 == s32_res)
        {
            perror("Socket send failed");
            goto LABEL_CLEAN_EXIT;
        }

        sleep(1);
    }

LABEL_CLEAN_EXIT:
    if (s32_sock > 0)
    {
        close(s32_sock);
    }

    printf("***** Raw Socket test- end\n");

    return EXIT_SUCCESS;
}