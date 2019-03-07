#include "write_data.h"

int openTextFile(char* fileName) {
    fp = fopen(fileName,"w");

    if (fp == NULL) {
        perror("Cannot open the file\n");

        return ERROR_OPEN_FILE;
    }

    return NO_ERROR;
}

void closeTextFile() {
    fclose(fp);
}

void writeDataToText(long networkLatency) {
    fprintf(fp,"%ld\n",networkLatency);
}