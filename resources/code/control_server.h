#include "main_header.h"
#include "common_header.h"

#define FROM_TARGET         0
#define NOT_FROM_TARGET     1

#define TOKEN_NUM           3

struct sockaddr_ll s_src_addr;
int32_t s32_sock;
uint16_t u16_data_off;
uint8_t *pu8a_frame;
uint8_t *pu8a_data;
socklen_t u32_src_addr_len;
char *sArr[3];

int init_server_frame(int frame_size);
void set_server_frame_header(uint8_t* src_addr, uint8_t* dest_addr);
void free_server_frame();

void init_server_data(int data_size);
void set_server_data(int data_size, int index, long timestamp);

void init_server_socket();
int set_server_socket();
int bind_server_socket();
void close_server_socket();

void init_server_sockaddr_ll();
void set_server_sockaddr_ll(int32_t nic_index);

int receive_data(int frame_size);
int check_data_from_target(uint8_t* target_address);

void parse_data();

char* get_packet_string();
char* get_packet_index();
char* get_packet_timestamp();

void print_target_mac_addr();