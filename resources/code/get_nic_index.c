#include "get_nic_index.h"

int32_t get_nic_index(char *nic_card_name)
{
    int32_t s32_sock_fd = -1;
    int32_t s32_res = -1;
    struct ifreq s_ifr;

    (void)memset(&s_ifr, 0, sizeof(s_ifr));

    s32_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (-1 == s32_sock_fd)
    {
        perror("get_nic_index(): socket failed");
        goto LABEL_CLEAN_EXIT;
    }

    s_ifr.ifr_addr.sa_family = AF_INET;

    strncpy(s_ifr.ifr_name, nic_card_name, IFNAMSIZ);

    printf("NIC : %s\n", nic_card_name);
    s32_res = ioctl(s32_sock_fd, SIOCGIFINDEX, &s_ifr);

    if (-1 == s32_res)
    {
        perror("get_nic_index(): ioctl failed");
    }

    close(s32_sock_fd);

LABEL_CLEAN_EXIT:
    printf("NIC INDEX : %d\n", s_ifr.ifr_ifru.ifru_ivalue);
    return (s_ifr.ifr_ifru.ifru_ivalue);
}

void get_nic_name()
{
    FILE *pFile = NULL;
    int i = 0;

    pFile = fopen("configuration.txt", "r");

    if (pFile != NULL)
    {
        char strTemp[255];
        fgets(strTemp, sizeof(strTemp), pFile);

        if (strTemp != NULL)
        {
            char *ptr = strtok(strTemp, ":");
            ptr = strtok(NULL, "\n");
            NIC_NAME = (char*)malloc(sizeof(char)*strlen(ptr));
            strcpy(NIC_NAME, ptr);
        }
    }
    else
    {
        printf("Incorrect format (NIC_NAME)\n");
        exit;
    }

    fclose(pFile);
}
