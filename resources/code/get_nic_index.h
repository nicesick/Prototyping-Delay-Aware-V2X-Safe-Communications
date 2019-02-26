#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <linux/if.h>
#include "common_header.h"

#define ERROR_SOCKET    -1
#define ERROR_IOCTL     -2

int32_t get_nic_index(char *nic_card_name);