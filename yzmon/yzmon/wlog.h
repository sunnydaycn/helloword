#ifndef wlog_h
#define wlog_h

#include<stdio.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>

#endif // wlog_h


int writelog(char * logName, int  num_s, char * data);

int writeDataInHex(char * logName, char * data_i, int length);
