#include "setup.h"

/*
Ex)
ip link set WLAN down
iw dev WLAN set type ocb
ip link set WLAN up
iw dev WLAN ocb join 5910 10MHZ
iw dev WLAN info
*/
extern char *NIC_NAME;
int bandwidth;
int frequency;

int main()
{
    char str[255];
    get_bandwidth_frequency();
    get_nic_name();

    sprintf(str, "/bin/bash -c 'ip link set %s down'", NIC_NAME);
    system(str);

    sprintf(str, "/bin/bash -c 'iw dev %s set type ocb'", NIC_NAME);
    system(str);
    sprintf(str, "/bin/bash -c 'ip link set %s up'", NIC_NAME);
    system(str);
    sprintf(str, "/bin/bash -c 'iw dev %s ocb join %d %dMHZ'", NIC_NAME, bandwidth, frequency);
    system(str);
    sprintf(str, "/bin/bash -c 'iw dev %s info'", NIC_NAME);
    system(str);

    return 0;
}

void get_bandwidth_frequency()
{
    FILE *pFile = NULL;
    int i = 0;

    pFile = fopen("configuration.txt", "r");

    if (pFile != NULL)
    {
        char strTemp[255];

        for (; i < 4; i++)
        {
            fgets(strTemp, sizeof(strTemp), pFile); // jump to 4th line
        }

        if (strTemp != NULL)
        {
            char *ptr = strtok(strTemp, ":");
            ptr = strtok(NULL, "\n");
            bandwidth = atoi(ptr);

            if (bandwidth < 0 || bandwidth > 10000)
            {
                printf("Incorrect format (bandwidth)\n");
                exit(0);
            }
        }

        fgets(strTemp, sizeof(strTemp), pFile);

        if (strTemp != NULL)
        {
            char *ptr = strtok(strTemp, ":");
            ptr = strtok(NULL, "\n");
            frequency = atoi(ptr);

            if (frequency < 0 || frequency > 300)
            {
                printf("Incorrect format (frequency)\n");
                exit(0);
            }
        }
    }
    else
    {
        printf("File Error\n");
        exit;
    }

    fclose(pFile);
}
