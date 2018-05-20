#pragma once
#ifndef AuthenRequest_h
#define AuthenRequest_h

#include "encrpty.h"
#include <stdio.h>
#include <string.h>

#endif 

int getRetryTime(char *msg);
int getReconnectTime(char *msg);
int getServerVersion(char *msg);
int getServerTime(char *msg);
int checkMsg(char *msg);

















