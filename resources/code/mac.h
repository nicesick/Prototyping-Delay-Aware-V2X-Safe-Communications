/*
 This header file to gain MAC_SRC and MAC_DST from configuration.txt
*/

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*our MAC address*/
uint8_t gu8a_src_mac[6];
/*other host MAC address*/
uint8_t gu8a_dest_mac[6];

void get_mac_addr();