/*
Coded by Hyunjae Lee and Jihun Lim

This header file is for including socket library
*/

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <linux/if.h>
#include <linux/if_arp.h>
#include <arpa/inet.h>
#include <time.h>

#define NO_ERROR                0

/*
This error code will be used in control_client.c and control_server.c
The function will return this error code according to below condifion

ERROR_CREATE_SOCKET : if creating socket is failed 
ERROR_CREATE_FRAME  : if creating frame is failed
ERROR_SEND_MESSAGE  : if sending the message is failed
ERROR_BIND_SOCKET   : if binding the socket is failed
ERROR_RECEIVE_DATA  : if receiving the data is failed
FAIL_RECEIVE_DATA   : if receiving the data is failed
*/

#define ERROR_CREATE_SOCKET     1
#define ERROR_CREATE_FRAME      2
#define ERROR_SEND_MESSAGE      3
#define ERROR_BIND_SOCKET       4
#define ERROR_RECEIVE_DATA      5
#define FAIL_RECEIVE_DATA       6

/*
This error code will be used in get_nic_index.c
The function will return this error code according to below condifion

ERROR_IOCTL : if finding NIC_INDEX is failed
*/

#define ERROR_IOCTL             7