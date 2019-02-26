#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <linux/if.h>
#include <linux/if_arp.h>
#include <arpa/inet.h>
#include <time.h>

#define NO_ERROR                0
#define ERROR_CREATE_SOCKET     1
#define ERROR_CREATE_FRAME      2
#define ERROR_SEND_MESSAGE      3