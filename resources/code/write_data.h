/*
Coded by Hyunjae Lee and Jihun Lim

This header file is for saving the latencys to text file
*/

#include "common_header.h"

FILE * fp;

int openTextFile(char* fileName);
void closeTextFile();

void writeDataToText(long networkLatency);