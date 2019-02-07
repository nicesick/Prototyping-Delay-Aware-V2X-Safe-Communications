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
#include "mac.h"
#include "get_nic_index.h"
#include "time_micro.h"

extern char *NIC_NAME;
extern uint8_t gu8a_src_mac[6];
extern uint8_t gu8a_dest_mac[6];

void *main()
{
    struct sockaddr_ll s_src_addr;
    int32_t s32_sock = -1;
    int32_t s32_res = -1;
    uint16_t u16_data_off = 0;
    uint8_t *pu8a_frame = NULL;
    uint16_t u16_i = 0;
    long lastTime, temp;
    char *sArr[3] = {
        NULL,
    };
    int flag = 0;
    get_mac_addr();
    printf("test) get_mac_addr = ");
    for (int i = 0; i < 6; i++)
    {
        printf("%x ", gu8a_src_mac[i]);
    }
    printf("\n");

    NIC_NAME = get_nic_name();
    printf("test) NIC_NAME = %s\n", NIC_NAME);

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

    s_src_addr.sll_family = AF_PACKET;
    /*we don't use a protocol above ethernet layer, just use anything here*/
    s_src_addr.sll_protocol = htons(ETH_P_ALL);
    printf("=before sll_ifindex = get_nic_index \n==");
    s_src_addr.sll_ifindex = get_nic_index((uint8_t *)NIC_NAME);
    printf("done\n");
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

            printf("Received data ");

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
                printf("Received data %s\n\n", &pu8a_frame[u16_data_off]);
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
                        lastTime = getMicrotime();
                        temp = atol(sArr[u16_i]);
                        lastTime -= temp;
                        printf("with latency %ld (ms) ", lastTime);
                    }
                }
                printf("\n");
                printf("==============================================\n");
            }
        }
    }

LABEL_CLEAN_EXIT:
    return (NULL);
}
