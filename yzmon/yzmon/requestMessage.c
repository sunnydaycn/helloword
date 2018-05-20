#include "requestMessage.h"

//���.h�������ɸ��ֱ���
short lowestVersion(char *msg)//������Ͱ汾����
{
	char temp_8;
	short temp_16;
	int temp_32;
	msg[0] = 0x91;
	msg[1] = 0x00;
	temp_16 = htons(12);
	memcpy(&msg[2], &temp_16, 2);//�ñ����ܳ����ǹ̶���12�ֽ�
	msg[4] = 0x00;
	msg[5] = 0x00;
	temp_16 = htons(4);
	memcpy(&msg[6], &temp_16, 2);//�ñ��ĵ����ݳ����ǹ̶���4�ֽ�
	temp_16 = 0x0000;
	memcpy(&msg[8], &temp_16, 2);//Ҫ�����ͷ������˰汾��  ��֪������ȥ��
	msg[10] = 0x00;//��1�汾��
	msg[11] = 0x00;//��2�汾��  ͬ����֪��ȥ�����

	return 12;
}

//����ֵΪ�����ܳ���
short confBaseMsg(int devid, char *msg_s,int *yibukou)//����������Ϣ����
{
	char temp_8;
	short temp_16;
	int temp_32;
	char msg[16384];
	short size_s = 0;//ͳ�Ʊ����ܳ���
	msg_s[size_s] = 0x91;
	size_s += 1;
	msg_s[size_s] = 0x01;
	size_s += 1;
	//�������ֽ��������
	size_s += 2;
	temp_16 = 0x0000;
	memcpy(&msg_s[size_s], &temp_16, 2);//�����ֽ�
	size_s += 2;
	//�߰��ֽ�Ҳ�����
	size_s += 2;


	//���Ĳ���
	short size = 0;
	short cpuHz = readCpuHZ();//cpuƵ��
	temp_16 = htons(cpuHz);
	memcpy(&msg[size], &temp_16, 2);
	size += 2;

	short memSize = readMemSize() / 1000;//ram��С ��λ��ΪMB
	temp_16 = htons(memSize);
	memcpy(&msg[size], &temp_16, 2);
	size += 2;

	temp_16 = htons(rand());
	memcpy(&msg[size], &temp_16, 2);//flash��С
	size += 2;

	temp_16 = htons(rand());
	memcpy(&msg[size], &temp_16, 2);//�豸�ڲ����к�
	size += 2;

	//16���ֽڵ������ַ�������豸�����к�
	char temp_c;
	int i;
	for (i = 0;i < 16;i++)
	{
		temp_c = rand() % ('z' - 'a') + 'a';
		memcpy(&msg[size + i], &temp_c, 1);
	}
	size += 16;

	//16���ֽڵ������ַ�������豸�ͺ�
	for (i = 0;i < 16;i++)
	{
		temp_c = rand() % ('z' - 'a') + 'a';
		memcpy(&msg[size + i], &temp_c, 1);
	}
	size += 16;


	//16���ֽڵ������ַ�������豸����汾��
	for (i = 0;i < 16;i++)
	{
		temp_c = rand() % ('z' - 'a') + 'a';
		memcpy(&msg[size + i], &temp_c, 1);
	}
	size += 16;



	//��̫������ һ�ֽ�
	temp_c = rand() % 3;
	memcpy(&msg[size], &temp_c, 1);
	size += 1;
	//ͬ�������� һ�ֽ�
	temp_c = rand() % 3;
	memcpy(&msg[size], &temp_c, 1);
	size += 1;
	//�첽������ һ�ֽ�
	*yibukou = 8 * (rand() % 3);
	memcpy(&msg[size], yibukou, 1);
	size += 1;
	//���������� һ�ֽ�
	temp_c = 8 * (rand() % 4);
	memcpy(&msg[size], &temp_c, 1);
	size += 1;
	//usb������ һ�ֽ�
	temp_c = rand() % 2;
	memcpy(&msg[size], &temp_c, 1);
	size += 1;
	//��ӡ������ һ�ֽ�
	temp_c = rand() % 2;
	memcpy(&msg[size], &temp_c, 1);
	size += 1;
	//pad һ�ֽ�
	char pid = rand();
	memcpy(&msg[size], &pid, 1);
	size += 1;
	//pad һ�ֽ�
	memcpy(&msg[size], &pid, 1);
	size += 1;

	//�豸�Ļ����� ���ֽ�
	temp_32 = htonl(devid);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	//��������� ����ֵ1��
	temp_c = 1;
	memcpy(&msg[size], &temp_c, 1);
	size += 1;
	//pad
	memcpy(&msg[size], &pid, 1);
	size += 1;
	//pad
	short pid_16 = pid;
	temp_16 = htons(pid_16);
	memcpy(&msg[size], &temp_16, 2);
	size += 2;


	//��֤��  32�ֽ�
	char str_[] = "yzmond:id*str&to!tongji@by#Auth^";
	memcpy(&msg[size], str_, 32);
	size += 32;

	//���ܱ���
	unsigned int random_num = (unsigned int)rand();
	int pos = random_num % 4093;
	encrpty_de(msg, size, pos);
	
	
	//�����Ĳ����뱨ͷ�������

	memcpy(&msg_s[size_s], msg, size);
	size_s += size;

	//��Կ ���ֽ�
	temp_32 = htonl(random_num);
	memcpy(&msg_s[size_s], &temp_32, 4);
	size_s += 4;

	//�������ֽڵı����ܳ���
	temp_16 = htons(size_s);
	memcpy(&msg_s[2], &temp_16, 2);

	//���߰��ֽڵı����ܳ���
	temp_16 = htons(size_s - 8);
	memcpy(&msg_s[6], &temp_16, 2);

	return size_s;
}

short sysMsg(char * msg)//ϵͳ��Ϣ����
{
	char temp_8;
	short temp_16;
	int temp_32;

	short size = 0;
	temp_16 = 0x0291;
	memcpy(&msg[size], &temp_16, 2);//���ĸ�ʽ��ʶ
	size += 2;

	temp_16 = htons(28);
	memcpy(&msg[size], &temp_16, 2);//�����ܳ��ȣ��̶�28�ֽ�
	size += 2;

	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//��֪����ɶ������ֽ�
	size += 2;

	temp_16 = htons(20);
	memcpy(&msg[size], &temp_16, 2);//���������ܳ��ȣ��̶�Ϊ20�ֽ�
	size += 2;


	//��ȡ���ݶεĲ���
	int userTime;
	int niceTime;
	int sysTime;
	int idleTime;
	int freedMem;
	readCpuTime(&userTime, &niceTime, &sysTime, &idleTime);
	freedMem = readFreedMem();

	temp_32 = htonl(userTime);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	temp_32 = htonl(niceTime);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	temp_32 = htonl(sysTime);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	temp_32 = htonl(idleTime);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	temp_32 = htonl(freedMem);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	return size;
}

short confMsg(char * msg)//������Ϣ����
{
	FILE *fp = fopen("config.dat", "r");
	if (fp == NULL)
	{
		printf("config.dat��ʧ��");
		return -1;
	}
	char confData[8092];
	int num = fread(confData, 1, 8091, fp);
	fclose(fp);
	confData[num] = '\0';

	char temp_8;
	short temp_16;
	int temp_32;

	short size = 0;
	temp_16 = 0x0391;
	memcpy(&msg[size], &temp_16, 2);//���ĵı�ʶ
	size += 2;
	temp_16 = htons(8 + num + 1);
	memcpy(&msg[size], &temp_16, 2);//�����ܳ��ȣ����а��ֽڵı�ͷ��num�ֽڵ����ݼ�һ�ֽڵ�β��
	size += 2;
	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//2�ֽڵ����0000
	size += 2;
	temp_16 = htons(num + 1);
	memcpy(&msg[size], &temp_16, 2);//���ݳ��ȣ�����num�����ݺ�һ��β��
	size += 2;
	memcpy(&msg[size], confData, num + 1);
	size += (num + 1);
	return size;
}

short proMsg(char * msg)//������Ϣ����
{
	FILE *fp = fopen("process.dat", "r");
	if (fp == NULL)
	{
		printf("process.dat��ʧ��");
		return -1;
	}
	char proData[8092];
	int num = fread(proData, 1, 8091, fp);
	fclose(fp);
	proData[num] = '\0';
	
	char temp_8;
	short temp_16;
	int temp_32;

	short size = 0;
	temp_16 = 0x0491;
	memcpy(&msg[size], &temp_16, 2);//���ĵı�ʶ
	size += 2;
	temp_16 = htons(8 + num + 1);
	memcpy(&msg[size], &temp_16, 2);//�����ܳ��ȣ����а��ֽڵı�ͷ��num�ֽڵ����ݼ�һ�ֽڵ�β��
	size += 2;
	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//2�ֽڵ����0000
	size += 2;
	temp_16 = htons(num + 1);
	memcpy(&msg[size], &temp_16, 2);//���ݳ��ȣ�����num�����ݺ�һ��β��
	size += 2;
	memcpy(&msg[size], proData, num + 1);
	size += (num + 1);
	return size;
}

short EthernetMsg(int id_Eth, char *msg)//������Ϣ����
{
	char temp_8;
	short temp_16;
	int temp_32;

	short size = 0;
	temp_16 = 0x0591;
	memcpy(&msg[size], &temp_16, 2);//���ı�ʶ
	size += 2;

	//�����ܳ��ȣ���ʱ��֪�����Ӻ���
	size += 2;

	temp_16 = htons(id_Eth);
	memcpy(&msg[size], &temp_16, 2);//�������ĸ�eth��
	size += 2;

	//���������ܳ��ȣ���ʱ��֪�����Ӻ���
	size += 2;

	temp_8 = 0x01;
	memcpy(&msg[size], &temp_8, 1);//�Ƿ����
	size += 1;

	temp_8 = 0x01;
	memcpy(&msg[size], &temp_8, 1);//�Ƿ�����
	size += 1;

	temp_8 = 0x01;
	memcpy(&msg[size], &temp_8, 1);//up or down 
	size += 1;

	temp_8 = 0x01;
	memcpy(&msg[size], &temp_8, 1);//pad
	size += 1;

	temp_8 = rand();
	memcpy(&msg[size], &temp_8, 1);//mac[0]
	size += 1;

	temp_8 = rand();
	memcpy(&msg[size], &temp_8, 1);//mac[1]
	size += 1;

	temp_8 = rand();
	memcpy(&msg[size], &temp_8, 1);//mac[2]
	size += 1;

	temp_8 = rand();
	memcpy(&msg[size], &temp_8, 1);//mac[3]
	size += 1;

	temp_8 = rand();
	memcpy(&msg[size], &temp_8, 1);//mac[4]
	size += 1;

	temp_8 = rand();
	memcpy(&msg[size], &temp_8, 1);//mac[5]
	size += 1;

	temp_16 = 0x0007;
	memcpy(&msg[size], &temp_16, 2);//options
	size += 2;

	int ip_n = rand();
	temp_32 = htonl(ip_n);
	memcpy(&msg[size], &temp_32, 4);//ip��ַ
	size += 4;
	temp_32 = htonl(0xffffff00);
	memcpy(&msg[size], &temp_32, 4);//��������
	size += 4;

	temp_32 = htonl(ip_n + 1);
	memcpy(&msg[size], &temp_32, 4);//����1 ip��ַ
	size += 4;
	temp_32 = htonl(0xffffff00);
	memcpy(&msg[size], &temp_32, 4);//����1 ��������
	size += 4;

	temp_32 = htonl(ip_n + 2);
	memcpy(&msg[size], &temp_32, 4);//����2 ip��ַ
	size += 4;
	temp_32 = htonl(0xffffff00);
	memcpy(&msg[size], &temp_32, 4);//����2 ��������
	size += 4;

	temp_32 = htonl(ip_n + 3);
	memcpy(&msg[size], &temp_32, 4);//����3 ip��ַ
	size += 4;
	temp_32 = htonl(0xffffff00);
	memcpy(&msg[size], &temp_32, 4);//����3 ��������
	size += 4;

	temp_32 = htonl(ip_n + 4);
	memcpy(&msg[size], &temp_32, 4);//����4 ip��ַ
	size += 4;
	temp_32 = htonl(0xffffff00);
	memcpy(&msg[size], &temp_32, 4);//����4 ��������
	size += 4;

	temp_32 = htonl(ip_n + 5);
	memcpy(&msg[size], &temp_32, 4);//����5 ip��ַ
	size += 4;
	temp_32 = htonl(0xffffff00);
	memcpy(&msg[size], &temp_32, 4);//����5 ��������
	size += 4;

	FILE *fp = fopen("/proc/net/dev", "r");
	if (fp == NULL)
		return -1;
	int i;
	char temp[124];
	for (i = 0;i < 3 + id_Eth;i++)
	{
		fscanf(fp, "%s", temp);
	}

	fclose(fp);

	int byteNum;

	//receive
	byteNum = atoi(&temp[7]);//bytes
	temp_32 = htonl(byteNum);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	byteNum = atoi(&temp[15]);//packets
	temp_32 = htonl(byteNum);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	byteNum = atoi(&temp[23]);//errs
	temp_32 = htonl(byteNum);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	byteNum = atoi(&temp[28]);//drop
	temp_32 = htonl(byteNum);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	byteNum = atoi(&temp[33]);//fifo
	temp_32 = htonl(byteNum);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	byteNum = atoi(&temp[38]);//frame
	temp_32 = htonl(byteNum);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	byteNum = atoi(&temp[44]);//compressed
	temp_32 = htonl(byteNum);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	byteNum = atoi(&temp[55]);//multicast
	temp_32 = htonl(byteNum);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;


	//Transmit
	byteNum = atoi(&temp[65]);//bytes
	temp_32 = htonl(byteNum);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	byteNum = atoi(&temp[74]);//packets
	temp_32 = htonl(byteNum);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	byteNum = atoi(&temp[82]);//errs
	temp_32 = htonl(byteNum);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	byteNum = atoi(&temp[87]);//drop
	temp_32 = htonl(byteNum);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	byteNum = atoi(&temp[92]);//fifo
	temp_32 = htonl(byteNum);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	byteNum = atoi(&temp[98]);//colls
	temp_32 = htonl(byteNum);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	byteNum = atoi(&temp[103]);//carrier
	temp_32 = htonl(byteNum);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	byteNum = atoi(&temp[11]);//compressed
	temp_32 = htonl(byteNum);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	short size_tm;
	size_tm = size;
	temp_16 = htons(size_tm);
	memcpy(&msg[2], &temp_16, 2);//�����ܳ���
	size_tm = size - 8;
	temp_16 = htons(size_tm);
	memcpy(&msg[6], &temp_16, 2);//���������ܳ���
	return size;
}

short usbMsg(char *msg)//usb��Ϣ����
{
	char temp_8;
	short temp_16;
	int temp_32;

	short size = 0;
	temp_16 = 0x0791;
	memcpy(&msg[size], &temp_16, 2);//���ĸ�ʽ��ʶ
	size += 2;

	temp_16 = htons(12);
	memcpy(&msg[size], &temp_16, 2);//�����ܳ��� �̶�12�ֽ�
	size += 2;

	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//����ֽ� 0x0000
	size += 2;

	temp_16 = htons(4);
	memcpy(&msg[size], &temp_16, 2);//���������ܳ��� �̶�4�ֽ�
	size += 2;

	temp_8 = rand() % 1;
	memcpy(&msg[size], &temp_8, 1);//�Ƿ����u��
	size += 1;

	temp_8 = rand();
	memcpy(&msg[size], &temp_8, 1);//pad
	size += 1;

	temp_8 = rand();
	memcpy(&msg[size], &temp_8, 1);//pad
	size += 1;

	temp_8 = rand();
	memcpy(&msg[size], &temp_8, 1);//pad
	size += 1;

	return size;
}

short usbFileMsg(char * msg)//������Ϣ����
{
	FILE *fp = fopen("usbfiles.dat", "r");
	if (fp == NULL)
	{
		printf("usbfiles.dat��ʧ��");
		return -1;
	}
	char usbFileData[4096];
	int num = fread(usbFileData, 1, 4095, fp);
	fclose(fp);
	usbFileData[num] = '\0';

	char temp_8;
	short temp_16;
	int temp_32;

	short size = 0;
	temp_16 = 0x0c91;
	memcpy(&msg[size], &temp_16, 2);//���ĵı�ʶ
	size += 2;
	temp_16 = htons(8 + num + 1);
	memcpy(&msg[size], &temp_16, 2);//�����ܳ��ȣ����а��ֽڵı�ͷ��num�ֽڵ����ݼ�һ�ֽڵ�β��
	size += 2;
	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//2�ֽڵ����0000
	size += 2;
	temp_16 = htons(num + 1);
	memcpy(&msg[size], &temp_16, 2);//���ݳ��ȣ�����num�����ݺ�һ��β��
	size += 2;
	memcpy(&msg[size], usbFileData, num + 1);
	size += (num + 1);
	return size;
}

short printfMsg(char *msg)
{
	char temp_8;
	short temp_16;
	int temp_32;

	short size = 0;
	temp_16 = 0x0891;
	memcpy(&msg[size], &temp_16, 2);//���ĸ�ʽ��ʶ
	size += 2;

	temp_16 = htons(44);
	memcpy(&msg[size], &temp_16, 2);//���ĳ��ȣ��̶�Ϊ32+4+8
	size += 2;

	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//����ֽ�
	size += 2;

	temp_16 = htons(36);
	memcpy(&msg[size], &temp_16, 2);//���ݶ��ܳ���
	size += 2;

	char on_off = rand() % 1;
	memcpy(&msg[size], &on_off, 1);//�����Ƿ�����
	size += 1;

	temp_8 = rand();
	memcpy(&msg[size], &temp_8, 1);//pad
	size += 1;

	if (on_off == 1)
		temp_16 = htons(rand() % 26);
	else
		temp_16 = 0;
	memcpy(&msg[size], &temp_16, 2);//��ӡ����������������
	size += 2;

	int i;
	for (i = 0;i < 32;i++) 
	{
		temp_8 = rand() % ('z' - 'a') + 'a';
		memcpy(&msg[size], &temp_8, 1);//��ӡ������
		size += 1;
	}
	return size;
}

short prinfListMsg(char *msg)
{
	char temp_8;
	short temp_16;
	int temp_32;

	short size = 0;
	temp_16 = 0x0d91;
	memcpy(&msg[size], &temp_16, 2);//���ĸ�ʽ��ʶ
	size += 2;

	temp_16 = htons(9);
	memcpy(&msg[size], &temp_16, 2);//���ĳ��ȣ��̶�Ϊ9
	size += 2;

	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//����ֽ�
	size += 2;

	temp_16 = htons(1);
	memcpy(&msg[size], &temp_16, 2);//���ݶ��ܳ��� �̶�Ϊ1
	size += 2;

	temp_8 = 0;
	memcpy(&msg[size], &temp_8, 1);//��ӡ����
	size += 1;

	return size;
}

short serverMsg(int yibukou, char *msg)
{
	char temp_8;
	short temp_16;
	int temp_32;

	short size = 0;
	temp_16 = 0x0991;
	memcpy(&msg[size], &temp_16, 2);//���ĸ�ʽ��ʶ
	size += 2;

	//temp_16 = htons(9);
	//memcpy(&msg[size], &temp_16, 2);//���ĳ���
	size += 2;

	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//����ֽ�
	size += 2;

	//temp_16 = htons(1);
	//memcpy(&msg[size], &temp_16, 2);//���ݶ��ܳ���
	size += 2;


	char value[20];
	char errmsg[100];
	int min_dev_num = 5;//��С�����ն�������ȱʡΪ5
	int max_dev_num = 28;//��������ն�������ȱʡΪ28
	if (readconf("ts.log", "��С�����ն�����", value, errmsg) != -1)
		min_dev_num = atoi(value);
	if (readconf("ts.log", "��������ն�����", value, errmsg) != -1)
		max_dev_num = atoi(value);
	if (min_dev_num>10 || min_dev_num<3)
		min_dev_num = 5;
	if (max_dev_num>50 || max_dev_num<10)
		max_dev_num = 28;

	int totla = (rand() % (max_dev_num - min_dev_num)) + min_dev_num;
	//int totla = (rand() % max_dev_num) + min_dev_num;


	int async_term_num;
	if (yibukou != 0)
		async_term_num = rand() % yibukou;	//int async_term_num = 4;
	else
		async_term_num = 0;


	if (async_term_num > totla)
		totla = async_term_num;

	int i;
	for (i = 0;i < 16;i++)//������ն�
	{
		if (i < async_term_num)
			temp_8 = 0x01;
		else
			temp_8 = 0x00;
		memcpy(&msg[size], &temp_8, 1);
		size += 1;
	}

	int ipter_num = totla - async_term_num;

	for (i = 0;i < 254;i++)
	{
		if (i < (ipter_num))
			temp_8 = 0x01;
		else
			temp_8 = 0x00;
		memcpy(&msg[size], &temp_8, 1);
		size += 1;
	}

	temp_16 = htons((rand() % (271 - totla)) + totla);
	memcpy(&msg[size], &temp_16, 2);
	size += 2;

	temp_16 = htons(size);
	memcpy(&msg[2], &temp_16, 2);//�����ܳ���
	temp_16 = htons(size-8);
	memcpy(&msg[6], &temp_16, 2);//���������ܳ���


	return size;
}

void virtualScreen(int id, int port, int ip, char *msg, short *size)
{
	char temp_8;
	short temp_16;
	int temp_32;

	temp_8 = id;
	memcpy(&msg[*size], &temp_8, 1);//�������
	*size += 1;

	temp_8 = rand();
	memcpy(&msg[*size], &temp_8, 1);//pad
	*size += 1;

	temp_16 = htons(port);
	memcpy(&msg[*size], &temp_16, 2);//Զ�˷������˿ں�
	*size += 2;

	temp_32 = htonl(ip);
	memcpy(&msg[*size], &temp_32, 4);//Զ�˷������˿ں�
	*size += 4;


	int sig = rand() % 2;
	char virtualProtocol[12];
	if (sig == 0)
		strcpy(virtualProtocol, "SSH");
	else
		strcpy(virtualProtocol, "ר��SSH");
	memcpy(&msg[*size], virtualProtocol, 12);//����Э��
	*size += 12;

	sig = rand() % 3;
	char virtualStatus[8];
	if (sig == 0)
		strcpy(virtualStatus, "����");
	else if (sig == 1)
		strcpy(virtualStatus, "�ػ�");
	else
		strcpy(virtualStatus, "�ѵ�¼");
	memcpy(&msg[*size], virtualProtocol, 8);//����״̬
	*size += 8;

	sig = rand() % 2;
	char virtualStr[24];
	if (sig == 0)
		strcpy(virtualStr, "����ϵͳ");
	else
		strcpy(virtualStr, "���𿪻�");
	memcpy(&msg[*size], virtualStr, 24);//������ʾ��
	*size += 24;


	sig = rand() % 2;
	char virtualType[12];
	if (sig == 0)
		strcpy(virtualStr, "vt100");
	else
		strcpy(virtualStr, "vt220");
	memcpy(&msg[*size], virtualStr, 12);//������Ӧ���ն�����
	*size += 12;

	int time_n = time();
	temp_32 = htonl(time_n);
	memcpy(&msg[*size], &temp_32, 4);//�ն����ӵ�ʱ��
	*size += 4;


	//�ĸ�ͳ����Ϣ
	temp_32 = htonl(rand());
	memcpy(&msg[*size], &temp_32, 4);
	*size += 4;

	temp_32 = htonl(rand());
	memcpy(&msg[*size], &temp_32, 4);
	*size += 4;

	temp_32 = htonl(rand());
	memcpy(&msg[*size], &temp_32, 4);
	*size += 4;

	temp_32 = htonl(rand());
	memcpy(&msg[*size], &temp_32, 4);
	*size += 4;


	//����pingֵ
	temp_32 = htonl((unsigned int)rand() % 123456);
	memcpy(&msg[*size], &temp_32, 4);
	*size += 4;

	temp_32 = htonl((unsigned int)rand() % 123456);
	memcpy(&msg[*size], &temp_32, 4);
	*size += 4;

	temp_32 = htonl((unsigned int)rand() % 123456);
	memcpy(&msg[*size], &temp_32, 4);
	*size += 4;


}


short virtualScreenMsg(char sig, short idTerminal, int port, int ip, char *msg,int *num_scr)
{
	char temp_8;
	short temp_16;
	int temp_32;

	short size = 0;
	temp_8 = 0x91;
	memcpy(&msg[size], &temp_8, 1);//���ĸ�ʽ��ʶ
	size += 1;

	temp_8 = sig;
	memcpy(&msg[size], &temp_8, 1);//���ĸ�ʽ��ʶ
	size += 1;

	//temp_16 = htons(9);
	//memcpy(&msg[size], &temp_16, 2);//���ĳ���
	size += 2;

	temp_16 = idTerminal;
	memcpy(&msg[size], &temp_16, 2);//����ֽ�
	size += 2;
	
	//temp_16 = htons(1);
	//memcpy(&msg[size], &temp_16, 2);//���ݶ��ܳ���
	size += 2;

	//temp_8 = rand();
	temp_8 = ntohs(idTerminal);
	memcpy(&msg[size], &temp_8, 1);//�˿ں�
	size += 1;

	//temp_8 = rand();
	temp_8 = ntohs(idTerminal);
	memcpy(&msg[size], &temp_8, 1);//���ö˿ں�
	size += 1;

	char value[20];
	char errmsg[100];
	int min_tty_num = 3;//ÿ���ն���С����������ȱʡΪ3
	int max_tty_num = 10;//ÿ���ն��������������ȱʡΪ10
	if (readconf("ts.log", "ÿ���ն���С��������", value, errmsg) != -1)
		min_tty_num = atoi(value);
	if (readconf("ts.log", "ÿ���ն������������", value, errmsg) != -1)
		max_tty_num = atoi(value);
	if (min_tty_num < 1 || min_tty_num>3)
		min_tty_num = 3;
	if (max_tty_num < 4 || max_tty_num>16)
		max_tty_num = 10;

	int screen_num = (rand() % (max_tty_num - min_tty_num)) + min_tty_num;
	//int screen_num = (rand() % max_tty_num) + min_tty_num;

	temp_8 = rand() % screen_num;
	memcpy(&msg[size], &temp_8, 1);//��ǰ�����
	size += 1;

	*num_scr = screen_num;
	temp_8 = screen_num;
	memcpy(&msg[size], &temp_8, 1);//��������
	size += 1;

	if (sig == 0x0a)
		temp_32 = 0x00;
	else
		temp_32 = rand();

	memcpy(&msg[size], &temp_32, 4);//�ն˵�ip��ַ
	size += 4;

	char typeTerminal[12];
	if (sig == 0x0a)
		strcpy(typeTerminal, "�����ն�");
	else if (rand() % 1 == 0)
		strcpy(typeTerminal, "IP����");
	else
		strcpy(typeTerminal, "IP�ն�");

	memcpy(&msg[size], typeTerminal, 12);//�ն�����
	size += 12;

	char statusTerminal[8];
	if (rand() % 1 == 0)
		strcpy(statusTerminal, "����");
	else
		strcpy(statusTerminal, "�˵�");

	memcpy(&msg[size], statusTerminal, 8);//�ն�״̬
	size += 8;

	int i;
	for (i = 0;i < screen_num;i++)
	{
		virtualScreen(i + 1, port, ip, msg, &size);
	}

	temp_16 = htons(size);
	memcpy(&msg[2], &temp_16, 2);//�����ܳ���
	temp_16 = htons(size - 8);
	memcpy(&msg[6], &temp_16, 2);//���������ܳ���

	return size;
}

short endMsg(char *msg)
{
	char temp_8;
	short temp_16;
	int temp_32;

	short size = 0;
	temp_16 = 0xff91;
	memcpy(&msg[size], &temp_16, 2);//���ĸ�ʽ��ʶ
	size += 2;

	temp_16 = htons(8);
	memcpy(&msg[size], &temp_16, 2);//���ĳ��ȣ��̶�Ϊ8
	size += 2;

	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//����ֽ�
	size += 2;

	temp_16 = htons(0);
	memcpy(&msg[size], &temp_16, 2);//���ݶ��ܳ��� �̶�Ϊ0
	size += 2;

}