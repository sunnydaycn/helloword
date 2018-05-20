#include "AuthenRequest.h"

//本.h文件是对认证请求的处理
int getRetryTime(char *msg)//从服务器发来的认证请求中获取重传时间
{
    short time;
	memcpy(&time, &msg[14], 2);
    return ntohs(time);
}

int getReconnectTime(char *msg)//从服务器发来的认证请求中获取 失败 重连时间
{
	short time;
	memcpy(&time, &msg[12], 2);
	return ntohs(time);
}

int getServerVersion(char *msg)//从服务器发来的认证请求中获取版本信息，如果低于0002.00.00 则返回-1；
{
	int version;
	memcpy(&version, &msg[8], 4);
	version = ntohl(version);
	if (version < 0x00020000)//版本信息低于0002.00.00的返回-1
		return -1;
	return 0;
}

int getServerTime(char *msg)
{
	int time;
    memcpy(&time,&msg[56],4);
	time = ntohl(time);
    time=time^0xFFFFFFFF;//将时间解密
    if(time<1483200000)//如果时间戳小于2017.1.1 00:00:00 则返回-1
        return -1;
    return 0;
}

int checkMsg(char *msg)
{
    int i;
    char right_msg[32]="yzmond:id*str&to!tongji@by#Auth^";//正确的认证报文
	char cerstring[32];
    int random_num;
    int pos;
    
	memcpy(cerstring,&msg[20],32);//读取报文
	//printf("%s\n", cerstring);
    memcpy(&random_num,&msg[52],4);//读取秘钥

	random_num = ntohl(random_num);//获取加密的秘钥

	pos=(random_num%4093);
	encrpty_de(cerstring, 32, pos);//解密
    if(strncmp(right_msg,cerstring,32)!=0)//如果认证串和要求的验证串不符合则返回-1
        return -1;
    return 0;
}


