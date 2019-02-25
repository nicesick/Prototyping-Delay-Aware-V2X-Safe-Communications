#include "common_header.h"
#include <net/ethernet.h> /* the L2 protocols */

char *packet_size;
int data_size, frame_size;

void set_packet_size(char *packet_size);
int get_packet_size(int option);