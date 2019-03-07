#include "packet_size.h"

int set_packet_size(char *packet_size)
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
        perror("You need to pur either SMALL or LARGE \n");
        return ERROR_WORD;
    }

    return NO_ERROR;
}

int get_packet_size(int option)
{
    if (option == FRAME) // frame
    {
        return frame_size;
    }
    else if (option == DATA) //data
    {
        return data_size;
    }
}