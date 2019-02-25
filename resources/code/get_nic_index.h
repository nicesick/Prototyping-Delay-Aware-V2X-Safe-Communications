#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <linux/if.h>
#include "common_header.h"

char *NIC_NAME;

int32_t get_nic_index(char *nic_card_name);
void get_nic_name();