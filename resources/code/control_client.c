#include "control_client.h"

int init_frame(int frame_size) {
    u16_data_off = (uint16_t) ETH_HLEN;
    pu8a_frame   = (uint8_t *) calloc(1, frame_size);

    if (pu8a_frame == NULL) {
        perror("Could not get memory for the transmit frame\n");
        return ERROR_CREATE_FRAME;
    }

    pu8a_data    = pu8a_frame + u16_data_off;

    return NO_ERROR;
}

void set_frame_header(uint8_t* src_addr, uint8_t* dest_addr) {
    (void)memcpy(pu8a_frame, dest_addr, ETH_ALEN);
    (void)memcpy(pu8a_frame + ETH_ALEN, src_addr, ETH_ALEN);
}

void free_frame() {
    free(pu8a_frame);
    pu8a_frame = NULL;
    pu8a_data  = NULL;
}

void init_data(int data_size) {
    (void)memset(&pu8a_frame[u16_data_off], '\0', data_size);
}

void set_data(int data_size, int index, long timestamp) {
    (void)snprintf((char *)&pu8a_frame[u16_data_off],
        data_size,"raw_packet_test %d %ld", index, timestamp);
}

void init_socket() {
    s32_sock = -1;
}

int set_socket() {
    s32_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if (s32_sock == -1) {
        perror("Could not create the socket");

        return ERROR_CREATE_SOCKET;
    }

    return NO_ERROR;
}

void close_socket() {
    if (s32_sock > 0) {
        close(s32_sock);
    }
}

void init_sockaddr_ll() {
    (void)memset(&s_dest_addr, 0, sizeof(s_dest_addr));
}

void set_sockaddr_ll(int32_t nic_index, uint8_t* dest_addr) {
    s_dest_addr.sll_family = AF_PACKET;
    /*we don't use a protocol above ethernet layer, just use anything here*/
    s_dest_addr.sll_protocol = htons(ETH_P_ALL);
    s_dest_addr.sll_ifindex = nic_index;
    s_dest_addr.sll_hatype = ARPHRD_ETHER;
    s_dest_addr.sll_pkttype = PACKET_OTHERHOST; //PACKET_OUTGOING
    s_dest_addr.sll_halen = ETH_ALEN;
    /*MAC - begin*/
    s_dest_addr.sll_addr[0] = dest_addr[0];
    s_dest_addr.sll_addr[1] = dest_addr[1];
    s_dest_addr.sll_addr[2] = dest_addr[2];
    s_dest_addr.sll_addr[3] = dest_addr[3];
    s_dest_addr.sll_addr[4] = dest_addr[4];
    s_dest_addr.sll_addr[5] = dest_addr[5];
    /*MAC - end*/
    s_dest_addr.sll_addr[6] = 0x00; /*not used*/
    s_dest_addr.sll_addr[7] = 0x00; /*not used*/
}

int send_data(int frame_size) {
    int sendResult = sendto(s32_sock,
                         pu8a_frame,
                         frame_size,
                         0,
                         (struct sockaddr *)&s_dest_addr,
                         sizeof(s_dest_addr));

    if (sendResult == -1) {
        perror("Socket send failed");

        return ERROR_SEND_MESSAGE;
    }

    return NO_ERROR;
}