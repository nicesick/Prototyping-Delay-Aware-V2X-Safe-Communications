#include "common_header.h"

#define MAC_ADDR_LEN        6

char* NIC_NAME;
int frequency;
int bandwidth;

/*our MAC address*/
uint8_t gu8a_src_mac[MAC_ADDR_LEN];
/*other host MAC address*/
uint8_t gu8a_dest_mac[MAC_ADDR_LEN];

int get_configuration();

char* get_nic_name();
uint8_t* get_src_addr();
uint8_t* get_dest_addr();

int get_frequency();
int get_bandwitch();
