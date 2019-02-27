#include "main_header.h"
#include "common_header.h"

#define TOKEN_NUM               3
#define MAX_DATA_LEN            100

#define FROM_TARGET             0
#define NOT_FROM_TARGET         1
#define CORRECT_DATA            2
#define NOT_CORRECT_DATA        3

struct sockaddr_ll s_src_addr;
int32_t s32_server_sock;
uint16_t u16_server_data_off;
uint8_t *pu8a_server_frame;
uint8_t *pu8a_server_data;
socklen_t u32_src_addr_len;
char *sArr[TOKEN_NUM];

int init_server_frame(int frame_size);
void free_server_frame();

void init_server_socket();
int set_server_socket();
int bind_server_socket();
void close_server_socket();

void init_server_sockaddr_ll();
void set_server_sockaddr_ll(int32_t nic_index);

int receive_data(int frame_size);
int check_data_from_target(uint8_t* target_address);

void parse_data();
int check_correct_data(char* string);

char* get_packet_string();
char* get_packet_index();
char* get_packet_timestamp();

void print_target_mac_addr();
void print_packet_string();
void print_packet_index();
void print_packet_timestamp();
