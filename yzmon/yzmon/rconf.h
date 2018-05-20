#ifndef rconf_h
#define rconf_h

#include <stdio.h>
#include <string.h>

#endif 

int readconf(char * fileName, char * parameter, char *value, char * errMsg);
short readCpuHZ();
int readMemSize();
int readCpuTime(int * userTime, int * niceTime, int * systemTime, int * idleTime);
int readFreedMem();
