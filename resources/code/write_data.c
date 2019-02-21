#include "write_data.h"

void openTextFile(char* fileName) {
    fp = fopen(fileName,"w");
}

void closeTextFile() {
    fclose(fp);
}

void writeDataToText(long networkLatency) {
    fprintf(fp,"%ld\n",networkLatency);
}