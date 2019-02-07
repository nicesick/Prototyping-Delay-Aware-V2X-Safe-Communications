/*
 This source file to gain MAC_SRC and MAC_DST from configuration.txt
*/

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mac.h"

void get_mac_addr()
{
    FILE *pFile = NULL;
    int i, j = 0;

    pFile = fopen("configuration.txt", "r");
    if (pFile != NULL)
    {
        char strTemp[255];

        while (!feof(pFile))
        {
            fgets(strTemp, sizeof(strTemp), pFile);
            if (i == 1)
            {
                char *ptr = strtok(strTemp, ":");
                while (1)
                {
                    ptr = strtok(NULL, ",");
                    if (ptr == NULL)
                        break;
                    gu8a_src_mac[j] = (uint8_t)strtol(ptr, NULL, 16);
                    j++;
                }
                j = 0;
            }
            else if (i == 2)
            {
                char *ptr = strtok(strTemp, ":");
                while (1)
                {
                    ptr = strtok(NULL, ",");
                    if (ptr == NULL)
                        break;
                    gu8a_dest_mac[j] = (uint8_t)strtol(ptr, NULL, 16);
                    j++;
                }
                j = 0;
            }
            i++;
        }
        fclose(pFile);
    }
    else
    {
        printf("Incorrect format (MAC_address)\n");
    }
}
