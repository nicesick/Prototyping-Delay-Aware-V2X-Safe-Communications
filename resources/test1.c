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


#define NIC_NAME "wlp4s0"


/*our MAC address*/
static uint8_t gu8a_src_mac[6]  = {0x04, 0xf0, 0x21, 0x36, 0x1b, 0x91};

/*other host MAC address*/
static uint8_t gu8a_dest_mac[6] = {0x04, 0xf0, 0x21, 0x36, 0x1b, 0x91};


int32_t get_nic_index (uint8_t *pu8_nic_card_name);

void* sock_recv_thread ();

int
main (void)
{
    struct sockaddr_ll  s_dest_addr;
    int32_t             s32_sock        = -1;
    int32_t             s32_res         = -1;
    pthread_t           ul_recv_thd_id  = -1;
    uint16_t            u16_data_off    = 0;
    uint16_t            u16_i           = 0;
    uint8_t             *pu8a_frame     = NULL;
    uint8_t             *pu8a_data      = NULL;

    printf ("Socket raw test\n");

    (void) memset (&s_dest_addr, 0, sizeof (s_dest_addr));

    pu8a_frame = (uint8_t *) calloc (ETH_FRAME_LEN, 1);

    if( NULL == pu8a_frame )
    {
        printf ("Could not get memory for the transmit frame\n");
        goto LABEL_CLEAN_EXIT;
    }

    u16_data_off = (uint16_t) (ETH_FRAME_LEN - ETH_DATA_LEN);

    pu8a_data = pu8a_frame + u16_data_off;

    s32_sock = socket (AF_PACKET, SOCK_RAW, htons (ETH_P_ALL));

    if( -1 == s32_sock )
    {
        perror ("Could not create the socket");
        goto LABEL_CLEAN_EXIT;
    }

    printf ("Sender) Socket created\n");

    fflush (stdout);

    (void) pthread_create (&ul_recv_thd_id, NULL, sock_recv_thread, NULL);

    sleep (1);

    s_dest_addr.sll_family      = AF_PACKET;
    /*we don't use a protocol above ethernet layer, just use anything here*/
    s_dest_addr.sll_protocol    = htons(ETH_P_ALL);
    s_dest_addr.sll_ifindex     = get_nic_index ((uint8_t *) NIC_NAME);
    s_dest_addr.sll_hatype      = ARPHRD_ETHER;
    s_dest_addr.sll_pkttype     = PACKET_OTHERHOST; //PACKET_OUTGOING
    s_dest_addr.sll_halen       = ETH_ALEN;
    /*MAC - begin*/
    s_dest_addr.sll_addr[0]     = gu8a_dest_mac[0];
    s_dest_addr.sll_addr[1]     = gu8a_dest_mac[1];
    s_dest_addr.sll_addr[2]     = gu8a_dest_mac[2];
    s_dest_addr.sll_addr[3]     = gu8a_dest_mac[3];
    s_dest_addr.sll_addr[4]     = gu8a_dest_mac[4];
    s_dest_addr.sll_addr[5]     = gu8a_dest_mac[5];
    /*MAC - end*/
    s_dest_addr.sll_addr[6]     = 0x00;/*not used*/
    s_dest_addr.sll_addr[7]     = 0x00;/*not used*/


    /* print START*/
    u16_i = 0;

    printf("s_dest_addr.sll_family    is %hd\n",s_dest_addr.sll_family );
    printf("s_dest_addr.sll_protocol  is %hd\n",s_dest_addr.sll_protocol );
    printf("s_dest_addr.sll_ifindex   is %d\n",s_dest_addr.sll_ifindex );
    printf("s_dest_addr.sll_hatype    is %hd\n",s_dest_addr.sll_hatype );
    printf("s_dest_addr.sll_pkttype   is %c\n",s_dest_addr.sll_pkttype );
    printf("s_dest_addr.sll_halen     is %c\n",s_dest_addr.sll_halen );
    printf("s_dest_addr.sll_addr      is %c\n",s_dest_addr.sll_halen );

    for( u16_i=0; u16_i<sizeof(s_dest_addr.sll_addr)-2; u16_i++ )
            {
                printf ("%02x:", s_dest_addr.sll_addr[u16_i]);
            }
    u16_i=0;
     /* print END*/

    /*set the frame header*/
    (void) memcpy (pu8a_frame, gu8a_dest_mac, ETH_ALEN);
    (void) memcpy (pu8a_frame+ETH_ALEN , gu8a_src_mac, ETH_ALEN);

    printf ("******Sending data using raw socket over '" NIC_NAME "'\n");

    while( 1 )
    {
        (void) memset (&pu8a_frame[u16_data_off], '\0', ETH_DATA_LEN);
        printf("Sender) memeset done \n");
        
        (void) snprintf ((char *) &pu8a_frame[u16_data_off],
                         ETH_DATA_LEN,
                         "raw packet test, %d", u16_i++);
        printf("Sender) snprintf done \n");

        s32_res = sendto (s32_sock,
                          pu8a_frame,
                          ETH_FRAME_LEN,
                          0,
                          (struct sockaddr*)&s_dest_addr,
                          sizeof(s_dest_addr));

        if( -1 == s32_res )
        {
            perror ("Socket send failed");
            goto LABEL_CLEAN_EXIT;
        }

        sleep (1);
    }

    /*printf ("Waiting for receive thread to exit\n");
    pthread_join (ul_recv_thd_id, NULL);*/

LABEL_CLEAN_EXIT:
    if( s32_sock > 0 )
    {
        close (s32_sock);
    }

    printf ("***** Raw Socket test- end\n");

    return EXIT_SUCCESS;
}

void*
sock_recv_thread ()
{
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

    printf ("Receiver) Socket created\n");
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
    
    /* print START*/
    

    printf("s_src_addr.sll_family    is %hd\n",s_src_addr.sll_family );
    printf("s_src_addr.sll_protocol  is %hd\n",s_src_addr.sll_protocol );
    printf("s_src_addr.sll_ifindex   is %d\n",s_src_addr.sll_ifindex );
    printf("s_src_addr.sll_hatype    is %hd\n",s_src_addr.sll_hatype );
    printf("s_src_addr.sll_pkttype   is %c\n",s_src_addr.sll_pkttype );
    printf("s_src_addr.sll_halen     is %c\n",s_src_addr.sll_halen );
    printf("s_src_addr.sll_addr      is ");
    for( u16_i=0; u16_i<sizeof(s_src_addr.sll_addr)-2; u16_i++ )
            {
                printf ("%02x:", s_src_addr.sll_addr[u16_i]);
            }
    printf("\n");

     /* print END*/

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
        printf("Receiver) memset done\n");

        printf("====test====\n");
        printf("s_sender_addr.sll_addr is ");
        for(int i = 0; i<8; i++){
           printf ("%02x:", s_sender_addr.sll_addr[i]);
        }
        printf("\n");
        
        s32_res = recvfrom (s32_sock,
                            pu8a_frame,
                            ETH_FRAME_LEN,
                            0,
                            (struct sockaddr *) &s_sender_addr,
                            &u32_sender_addr_len);
        printf("Receiver) recvfrom done\n");
        printf("====test====\n");
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


int32_t
get_nic_index (uint8_t *pu8_nic_card_name)
{
    int32_t         s32_sock_fd     = -1;
    int32_t         s32_res         = -1;
    struct  ifreq   s_ifr;

    (void) memset (&s_ifr, 0, sizeof (s_ifr));

    s32_sock_fd = socket (AF_INET, SOCK_DGRAM, 0);

    if( -1 == s32_sock_fd )
    {
        perror ("get_nic_index(): socket failed");
        goto LABEL_CLEAN_EXIT;
    }

    s_ifr.ifr_addr.sa_family = AF_INET;

    strncpy(s_ifr.ifr_name, (char *) pu8_nic_card_name, IFNAMSIZ);

    printf("NIC : %s\n",pu8_nic_card_name);
    s32_res = ioctl(s32_sock_fd, SIOCGIFINDEX, &s_ifr);

    if( -1 == s32_res )
    {
        perror ("get_nic_index(): ioctl failed");
    }

    close (s32_sock_fd);

LABEL_CLEAN_EXIT:
    printf ("NIC INDEX : %d\n",s_ifr.ifr_ifru.ifru_ivalue);
    return (s_ifr.ifr_ifru.ifru_ivalue);
}
