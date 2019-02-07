/*
 This header file to gain MAC_SRC and MAC_DST from configuration.txt
*/

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

/*our MAC address*/
extern uint8_t gu8a_src_mac[6];
/*other host MAC address*/
extern uint8_t gu8a_dest_mac[6];

void get_mac_addr();