#pragma once
#ifndef client_h
#define client_h
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<time.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include<net/if.h>
#include<sys/ioctl.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<signal.h>
#include <sys/prctl.h>

#include "requestMessage.h"
#include "AuthenRequest.h"
#include "rconf.h"
#include "wlog.h"
#endif // DEBUG


#define BUFF_SIZE 16384*4
