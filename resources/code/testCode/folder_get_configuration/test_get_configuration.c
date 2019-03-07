#include "get_configuration.h"

void main(void) {
    int resultConfiguration = get_configuration();

    if(resultConfiguration == NO_ERROR) {
        printf("NIC_NAME : %s\n",get_nic_name());

        printf("SRC_MAC : ");
        for(int index = 0; index < MAC_ADDR_LEN; index++) {
            printf("%x ",get_src_addr()[index]);
        }

        printf("\n");

        printf("DEST_MAC : ");
        for(int index = 0; index < MAC_ADDR_LEN; index++) {
            printf("%x ",get_dest_addr()[index]);
        }

        printf("\n");

        printf("FREQUENCY : %d\n",get_frequency());
        printf("BANDWIDTH : %d\n",get_bandwitch());
    }
}