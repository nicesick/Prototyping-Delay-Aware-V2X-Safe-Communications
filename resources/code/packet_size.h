#include "common_header.h"
#include <net/ethernet.h> /* the L2 protocols */

#define FRAME       1
#define DATA        2

int data_size, frame_size;

int set_packet_size(char *packet_size);
int get_packet_size(int option);