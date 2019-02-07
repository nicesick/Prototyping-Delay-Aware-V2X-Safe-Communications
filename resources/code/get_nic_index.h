#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <linux/if.h>

#define NIC_NAME "wlp4s0"

int32_t get_nic_index (uint8_t *nic_card_name);
