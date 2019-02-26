#include "main_header.h"
#include "common_header.h"

struct sockaddr_ll s_dest_addr;
int32_t s32_sock;
uint16_t u16_data_off;
uint8_t *pu8a_frame;
uint8_t *pu8a_data;

int init_frame(int frame_size);
void set_frame_header(uint8_t* src_addr, uint8_t* dest_addr);
void free_frame();

void init_data(int data_size);
void set_data(int data_size, int index, long timestamp);

void init_socket();
int set_socket();
void close_socket();

void init_sockaddr_ll();
void set_sockaddr_ll(int32_t nic_index, uint8_t* dest_addr);

int send_data(int frame_size);