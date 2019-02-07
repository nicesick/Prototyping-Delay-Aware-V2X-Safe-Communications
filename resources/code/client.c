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
#include "mac.h" /* MAC address */
#include "get_nic_index.h"
#include "time_micro.h"

int main(void)
{

    struct sockaddr_ll s_dest_addr;
    int32_t s32_sock = -1;
    int32_t s32_res = -1;
    uint16_t u16_data_off = 0;
    uint16_t u16_i = 0;
    uint8_t *pu8a_frame = NULL;
    uint8_t *pu8a_data = NULL;
    long curTime;

    get_mac_addr();
    printf("test) get_mac_addr = ");
    for (int i = 0; i < 6; i++)
    {
        printf("%x ", gu8a_src_mac[i]);
    }
    printf("\n");

    NIC_NAME = get_nic_name();
    printf("test) NIC_NAME = %s\n", NIC_NAME);

    printf("Socket raw test\n");

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

    s_dest_addr.sll_family = AF_PACKET;
    /*we don't use a protocol above ethernet layer, just use anything here*/
    s_dest_addr.sll_protocol = htons(ETH_P_ALL);
    printf("=before sll_ifindex = get_nic_index \n==");
    s_dest_addr.sll_ifindex = get_nic_index((uint8_t *)NIC_NAME);
    s_dest_addr.sll_hatype = ARPHRD_ETHER;
    s_dest_addr.sll_pkttype = PACKET_OTHERHOST; //PACKET_OUTGOING
    s_dest_addr.sll_halen = ETH_ALEN;
    /*MAC - begin*/
    printf("=before sll_addr = gu8a_dest_mac \n==");
    s_dest_addr.sll_addr[0] = gu8a_dest_mac[0];
    s_dest_addr.sll_addr[1] = gu8a_dest_mac[1];
    s_dest_addr.sll_addr[2] = gu8a_dest_mac[2];
    s_dest_addr.sll_addr[3] = gu8a_dest_mac[3];
    s_dest_addr.sll_addr[4] = gu8a_dest_mac[4];
    s_dest_addr.sll_addr[5] = gu8a_dest_mac[5];
    printf("done\n");
    /*MAC - end*/
    s_dest_addr.sll_addr[6] = 0x00; /*not used*/
    s_dest_addr.sll_addr[7] = 0x00; /*not used*/

    (void)memcpy(pu8a_frame, gu8a_dest_mac, ETH_ALEN);
    (void)memcpy(pu8a_frame + ETH_ALEN, gu8a_src_mac, ETH_ALEN);

    printf("******Sending data using raw socket over  %s \n", NIC_NAME);

    while (1)
    {
        (void)memset(&pu8a_frame[u16_data_off], '\0', ETH_DATA_LEN);

        curTime = getMicrotime();

        (void)snprintf((char *)&pu8a_frame[u16_data_off],
                       ETH_DATA_LEN,
                       "raw_packet_test %d %ld", u16_i++, curTime);

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
