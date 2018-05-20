#include "AuthenRequest.h"

//��.h�ļ��Ƕ���֤����Ĵ���
int getRetryTime(char *msg)//�ӷ�������������֤�����л�ȡ�ش�ʱ��
{
    short time;
	memcpy(&time, &msg[14], 2);
    return ntohs(time);
}

int getReconnectTime(char *msg)//�ӷ�������������֤�����л�ȡ ʧ�� ����ʱ��
{
	short time;
	memcpy(&time, &msg[12], 2);
	return ntohs(time);
}

int getServerVersion(char *msg)//�ӷ�������������֤�����л�ȡ�汾��Ϣ���������0002.00.00 �򷵻�-1��
{
	int version;
	memcpy(&version, &msg[8], 4);
	version = ntohl(version);
	if (version < 0x00020000)//�汾��Ϣ����0002.00.00�ķ���-1
		return -1;
	return 0;
}

int getServerTime(char *msg)
{
	int time;
    memcpy(&time,&msg[56],4);
	time = ntohl(time);
    time=time^0xFFFFFFFF;//��ʱ�����
    if(time<1483200000)//���ʱ���С��2017.1.1 00:00:00 �򷵻�-1
        return -1;
    return 0;
}

int checkMsg(char *msg)
{
    int i;
    char right_msg[32]="yzmond:id*str&to!tongji@by#Auth^";//��ȷ����֤����
	char cerstring[32];
    int random_num;
    int pos;
    
	memcpy(cerstring,&msg[20],32);//��ȡ����
	//printf("%s\n", cerstring);
    memcpy(&random_num,&msg[52],4);//��ȡ��Կ

	random_num = ntohl(random_num);//��ȡ���ܵ���Կ

	pos=(random_num%4093);
	encrpty_de(cerstring, 32, pos);//����
    if(strncmp(right_msg,cerstring,32)!=0)//�����֤����Ҫ�����֤���������򷵻�-1
        return -1;
    return 0;
}


