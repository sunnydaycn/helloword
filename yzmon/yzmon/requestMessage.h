#pragma once
#ifndef requestMessage_h
#define requestMessage_h

#include <stdio.h>
#include <stdlib.h>
#include "rconf.h"
#include "encrpty.h"

#endif // requestMessage_h


short lowestVersion(char *msg);

short confBaseMsg(int devid, char *msg, int *yibukou);

short sysMsg(char * msg);

short confMsg(char * msg);

short proMsg(char * msg);

short EthernetMsg(int id_Eth, char * msg);

short usbMsg(char * msg);

short usbFileMsg(char * msg);

short printfMsg(char * msg);

short prinfListMsg(char * msg);

short serverMsg(int yibukou, char * msg);

//short virtualScreenMsg(char sig, short idTerminal, int port, int ip, char * msg);

short virtualScreenMsg(char sig, short idTerminal, int port, int ip, char * msg, int * num_scr);

short endMsg(char * msg);
