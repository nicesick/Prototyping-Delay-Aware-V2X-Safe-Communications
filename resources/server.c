/*
 Coded by Hyunjae Lee and Jihun Lim
 
 This is a simple implementation for latency evaluation in a V2V network.
 We assume that all node knows other MAC addresses who want to communicate $

 [Server]
 There are a receiver in main function.
 The receiver is waiting until the receiver get some message with link-layer header.
 Whenever the receiver get the message from clients,
 the receiver show user to see link-layer header and payload.

  *Reference : https://stackoverflow.com/questions/10824827/raw-sockets-com$
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <linux/if.h>
#include <linux/if_arp.h>
#include <arpa/inet.h>

#include "get_nic_index.h"

void* main() {
struct sockaddr_ll  s_src_addr;
    int32_t             s32_sock        = -1;
    int32_t             s32_res         = -1;
    uint16_t            u16_data_off    = 0;
    uint8_t             *pu8a_frame     = NULL;
    uint16_t u16_i = 0;

    printf ("Socket receive thread\n");

    u16_data_off = (uint16_t) (ETH_FRAME_LEN - ETH_DATA_LEN);

    pu8a_frame = (uint8_t*) calloc (ETH_FRAME_LEN, 1);
    
    s32_sock = socket (AF_PACKET, SOCK_RAW, htons (ETH_P_ALL));

    if( -1 == s32_sock )
    {
        perror ("Could not create the socket");
        goto LABEL_CLEAN_EXIT;
    }

    //printf ("Receiver) Socket created\n");
    if( NULL == pu8a_frame )
    {
        printf ("Could not get memory for the receive frame\n");
        goto LABEL_CLEAN_EXIT;
    }

    (void) memset (&s_src_addr, 0, sizeof (s_src_addr));

    s_src_addr.sll_family       = AF_PACKET;
    /*we don't use a protocol above ethernet layer, just use anything here*/
    s_src_addr.sll_protocol     = htons(ETH_P_ALL);
    s_src_addr.sll_ifindex      = get_nic_index ((uint8_t *) NIC_NAME);
    s_src_addr.sll_hatype       = ARPHRD_ETHER;
    s_src_addr.sll_pkttype      = PACKET_HOST;//PACKET_OTHERHOST;
    s_src_addr.sll_halen        = ETH_ALEN;
    
    // /* print START*/
    

    // printf("s_src_addr.sll_family    is %hd\n",s_src_addr.sll_family );
    // printf("s_src_addr.sll_protocol  is %hd\n",s_src_addr.sll_protocol );
    // printf("s_src_addr.sll_ifindex   is %d\n",s_src_addr.sll_ifindex );
    // printf("s_src_addr.sll_hatype    is %hd\n",s_src_addr.sll_hatype );
    // printf("s_src_addr.sll_pkttype   is %c\n",s_src_addr.sll_pkttype );
    // printf("s_src_addr.sll_halen     is %c\n",s_src_addr.sll_halen );
    // printf("s_src_addr.sll_addr      is ");
    // for( u16_i=0; u16_i<sizeof(s_src_addr.sll_addr)-2; u16_i++ )
    //         {
    //             printf ("%02x:", s_src_addr.sll_addr[u16_i]);
    //         }
    // printf("\n");

    //  /* print END*/

    s32_res = bind (s32_sock,
                    (struct sockaddr *) &s_src_addr,
                    sizeof(s_src_addr));


    if( -1 == s32_res )
    {
        perror ("Could not bind to the socket");
        goto LABEL_CLEAN_EXIT;
    }

    printf ("Socket bind successful\n");

    while( 1 )
    {
        struct sockaddr_ll  s_sender_addr;
        socklen_t           u32_sender_addr_len = sizeof (s_sender_addr);

        (void) memset (&s_sender_addr, 0, sizeof (s_sender_addr));
        //printf("Receiver) memset done\n");

        //printf("====test====\n");
        //printf("s_sender_addr.sll_addr is ");
        //for(int i = 0; i<8; i++){
        //   printf ("%02x:", s_sender_addr.sll_addr[i]);
        //}
        //printf("\n");
        
        s32_res = recvfrom (s32_sock,
                            pu8a_frame,
                            ETH_FRAME_LEN,
                            0,
                            (struct sockaddr *) &s_sender_addr,
                            &u32_sender_addr_len);
        //printf("Receiver) recvfrom done\n");
        //printf("====test====\n");
        if( -1 == s32_res )
        {
            perror ("Socket receive failed");
            break;
        }
        else if( s32_res < 0 )
        {
            perror ("Socket receive, error ");
        }
        else
        {
            u16_i = 0;

            printf ("Received data from ");

            for( u16_i=0; u16_i<sizeof(s_sender_addr.sll_addr)-2; u16_i++ )
            {
                printf ("%02x:", s_sender_addr.sll_addr[u16_i]);
            }

            printf ("\t");

            printf ("Received data %s\n\n", &pu8a_frame[u16_data_off]);
        }

    }

LABEL_CLEAN_EXIT:
    return (NULL);

}
