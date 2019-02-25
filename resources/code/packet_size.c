#include "packet_size.h"

void set_packet_size(char *packet_size)
{
    if (strcmp(packet_size, "SMALL") == 0)
    {
        frame_size = ETH_FRAME_LEN;
        data_size = ETH_FRAME_LEN - ETH_HLEN;
    }
    else if (strcmp(packet_size, "LARGE") == 0)
    {
        frame_size = ETH_ZLEN;
        data_size = ETH_ZLEN - ETH_HLEN;
    }
    else
    {
        printf("You need to pur either SMALL or LARGE \n");
        exit(1);
    }
}

int get_packet_size(int option)
{
    if (option == 1) // frame
    {
        return frame_size;
    }
    else if (option == 2) //data
    {
        return data_size;
    }
}