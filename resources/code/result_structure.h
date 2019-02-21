#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define MAXIMUM 10

struct timeGap {
    long sendTime;
    long recvTime;
    long diff;
};

struct timeGap circular_array[MAXIMUM];

void initArray();

void putSendTime(long sendTime, int index);
void putRecvTime(long recvTime, int index);
void putDiff(long diff, int index);

long getSendTime(int index);
long getRecvTime(int index);
long getDiff(int index);

long getMessageLatency(int index);
long getNetworkLatency(int index);

void printAllArray();

long timespec_diff(long start, long stop);