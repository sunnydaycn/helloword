#include "requestMessage.h"

//这个.h用来生成各种报文
short lowestVersion(char *msg)//生成最低版本报文
{
	char temp_8;
	short temp_16;
	int temp_32;
	msg[0] = 0x91;
	msg[1] = 0x00;
	temp_16 = htons(12);
	memcpy(&msg[2], &temp_16, 2);//该报文总长度是固定的12字节
	msg[4] = 0x00;
	msg[5] = 0x00;
	temp_16 = htons(4);
	memcpy(&msg[6], &temp_16, 2);//该报文的数据长度是固定的4字节
	temp_16 = 0x0000;
	memcpy(&msg[8], &temp_16, 2);//要求的最低服务器端版本号  不知道从哪去读
	msg[10] = 0x00;//次1版本号
	msg[11] = 0x00;//次2版本号  同样不知道去哪里读

	return 12;
}

//返回值为报文总长度
short confBaseMsg(int devid, char *msg_s,int *yibukou)//基础配置信息报文
{
	char temp_8;
	short temp_16;
	int temp_32;
	char msg[16384];
	short size_s = 0;//统计报文总长度
	msg_s[size_s] = 0x91;
	size_s += 1;
	msg_s[size_s] = 0x01;
	size_s += 1;
	//第三四字节最后再填
	size_s += 2;
	temp_16 = 0x0000;
	memcpy(&msg_s[size_s], &temp_16, 2);//五六字节
	size_s += 2;
	//七八字节也最后填
	size_s += 2;


	//报文部分
	short size = 0;
	short cpuHz = readCpuHZ();//cpu频率
	temp_16 = htons(cpuHz);
	memcpy(&msg[size], &temp_16, 2);
	size += 2;

	short memSize = readMemSize() / 1000;//ram大小 单位化为MB
	temp_16 = htons(memSize);
	memcpy(&msg[size], &temp_16, 2);
	size += 2;

	temp_16 = htons(rand());
	memcpy(&msg[size], &temp_16, 2);//flash大小
	size += 2;

	temp_16 = htons(rand());
	memcpy(&msg[size], &temp_16, 2);//设备内部序列号
	size += 2;

	//16个字节的任意字符串组成设备组序列号
	char temp_c;
	int i;
	for (i = 0;i < 16;i++)
	{
		temp_c = rand() % ('z' - 'a') + 'a';
		memcpy(&msg[size + i], &temp_c, 1);
	}
	size += 16;

	//16个字节的任意字符串组成设备型号
	for (i = 0;i < 16;i++)
	{
		temp_c = rand() % ('z' - 'a') + 'a';
		memcpy(&msg[size + i], &temp_c, 1);
	}
	size += 16;


	//16个字节的任意字符串组成设备软件版本号
	for (i = 0;i < 16;i++)
	{
		temp_c = rand() % ('z' - 'a') + 'a';
		memcpy(&msg[size + i], &temp_c, 1);
	}
	size += 16;



	//以太口数量 一字节
	temp_c = rand() % 3;
	memcpy(&msg[size], &temp_c, 1);
	size += 1;
	//同步口数量 一字节
	temp_c = rand() % 3;
	memcpy(&msg[size], &temp_c, 1);
	size += 1;
	//异步口数量 一字节
	*yibukou = 8 * (rand() % 3);
	memcpy(&msg[size], yibukou, 1);
	size += 1;
	//交换口数量 一字节
	temp_c = 8 * (rand() % 4);
	memcpy(&msg[size], &temp_c, 1);
	size += 1;
	//usb口数量 一字节
	temp_c = rand() % 2;
	memcpy(&msg[size], &temp_c, 1);
	size += 1;
	//打印口数量 一字节
	temp_c = rand() % 2;
	memcpy(&msg[size], &temp_c, 1);
	size += 1;
	//pad 一字节
	char pid = rand();
	memcpy(&msg[size], &pid, 1);
	size += 1;
	//pad 一字节
	memcpy(&msg[size], &pid, 1);
	size += 1;

	//设备的机构号 四字节
	temp_32 = htonl(devid);
	memcpy(&msg[size], &temp_32, 4);
	size += 4;

	//机构内序号 （定值1）
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


	//认证串  32字节
	char str_[] = "yzmond:id*str&to!tongji@by#Auth^";
	memcpy(&msg[size], str_, 32);
	size += 32;

	//加密报文
	unsigned int random_num = (unsigned int)rand();
	int pos = random_num % 4093;
	encrpty_de(msg, size, pos);
	
	
	//将报文部分与报头结合起来

	memcpy(&msg_s[size_s], msg, size);
	size_s += size;

	//秘钥 四字节
	temp_32 = htonl(random_num);
	memcpy(&msg_s[size_s], &temp_32, 4);
	size_s += 4;

	//填三四字节的报文总长度
	temp_16 = htons(size_s);
	memcpy(&msg_s[2], &temp_16, 2);

	//填七八字节的报文总长度
	temp_16 = htons(size_s - 8);
	memcpy(&msg_s[6], &temp_16, 2);

	return size_s;
}

short sysMsg(char * msg)//系统信息报文
{
	char temp_8;
	short temp_16;
	int temp_32;

	short size = 0;
	temp_16 = 0x0291;
	memcpy(&msg[size], &temp_16, 2);//报文格式标识
	size += 2;

	temp_16 = htons(28);
	memcpy(&msg[size], &temp_16, 2);//报文总长度，固定28字节
	size += 2;

	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//不知道是啥的填充字节
	size += 2;

	temp_16 = htons(20);
	memcpy(&msg[size], &temp_16, 2);//报文数据总长度，固定为20字节
	size += 2;


	//读取数据段的参数
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

short confMsg(char * msg)//配置信息报文
{
	FILE *fp = fopen("config.dat", "r");
	if (fp == NULL)
	{
		printf("config.dat打开失败");
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
	memcpy(&msg[size], &temp_16, 2);//报文的标识
	size += 2;
	temp_16 = htons(8 + num + 1);
	memcpy(&msg[size], &temp_16, 2);//报文总长度，其中八字节的报头，num字节的数据加一字节的尾零
	size += 2;
	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//2字节的填充0000
	size += 2;
	temp_16 = htons(num + 1);
	memcpy(&msg[size], &temp_16, 2);//数据长度，包括num个数据和一个尾零
	size += 2;
	memcpy(&msg[size], confData, num + 1);
	size += (num + 1);
	return size;
}

short proMsg(char * msg)//进程信息报文
{
	FILE *fp = fopen("process.dat", "r");
	if (fp == NULL)
	{
		printf("process.dat打开失败");
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
	memcpy(&msg[size], &temp_16, 2);//报文的标识
	size += 2;
	temp_16 = htons(8 + num + 1);
	memcpy(&msg[size], &temp_16, 2);//报文总长度，其中八字节的报头，num字节的数据加一字节的尾零
	size += 2;
	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//2字节的填充0000
	size += 2;
	temp_16 = htons(num + 1);
	memcpy(&msg[size], &temp_16, 2);//数据长度，包括num个数据和一个尾零
	size += 2;
	memcpy(&msg[size], proData, num + 1);
	size += (num + 1);
	return size;
}

short EthernetMsg(int id_Eth, char *msg)//网卡信息报文
{
	char temp_8;
	short temp_16;
	int temp_32;

	short size = 0;
	temp_16 = 0x0591;
	memcpy(&msg[size], &temp_16, 2);//报文标识
	size += 2;

	//报文总长度，暂时不知道，延后填
	size += 2;

	temp_16 = htons(id_Eth);
	memcpy(&msg[size], &temp_16, 2);//区分是哪个eth口
	size += 2;

	//报文数据总长度，暂时不知道，延后填
	size += 2;

	temp_8 = 0x01;
	memcpy(&msg[size], &temp_8, 1);//是否存在
	size += 1;

	temp_8 = 0x01;
	memcpy(&msg[size], &temp_8, 1);//是否配置
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
	memcpy(&msg[size], &temp_32, 4);//ip地址
	size += 4;
	temp_32 = htonl(0xffffff00);
	memcpy(&msg[size], &temp_32, 4);//子网掩码
	size += 4;

	temp_32 = htonl(ip_n + 1);
	memcpy(&msg[size], &temp_32, 4);//子网1 ip地址
	size += 4;
	temp_32 = htonl(0xffffff00);
	memcpy(&msg[size], &temp_32, 4);//子网1 子网掩码
	size += 4;

	temp_32 = htonl(ip_n + 2);
	memcpy(&msg[size], &temp_32, 4);//子网2 ip地址
	size += 4;
	temp_32 = htonl(0xffffff00);
	memcpy(&msg[size], &temp_32, 4);//子网2 子网掩码
	size += 4;

	temp_32 = htonl(ip_n + 3);
	memcpy(&msg[size], &temp_32, 4);//子网3 ip地址
	size += 4;
	temp_32 = htonl(0xffffff00);
	memcpy(&msg[size], &temp_32, 4);//子网3 子网掩码
	size += 4;

	temp_32 = htonl(ip_n + 4);
	memcpy(&msg[size], &temp_32, 4);//子网4 ip地址
	size += 4;
	temp_32 = htonl(0xffffff00);
	memcpy(&msg[size], &temp_32, 4);//子网4 子网掩码
	size += 4;

	temp_32 = htonl(ip_n + 5);
	memcpy(&msg[size], &temp_32, 4);//子网5 ip地址
	size += 4;
	temp_32 = htonl(0xffffff00);
	memcpy(&msg[size], &temp_32, 4);//子网5 子网掩码
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
	memcpy(&msg[2], &temp_16, 2);//报文总长度
	size_tm = size - 8;
	temp_16 = htons(size_tm);
	memcpy(&msg[6], &temp_16, 2);//报文数据总长度
	return size;
}

short usbMsg(char *msg)//usb信息报文
{
	char temp_8;
	short temp_16;
	int temp_32;

	short size = 0;
	temp_16 = 0x0791;
	memcpy(&msg[size], &temp_16, 2);//报文格式标识
	size += 2;

	temp_16 = htons(12);
	memcpy(&msg[size], &temp_16, 2);//报文总长度 固定12字节
	size += 2;

	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//填充字节 0x0000
	size += 2;

	temp_16 = htons(4);
	memcpy(&msg[size], &temp_16, 2);//报文数据总长度 固定4字节
	size += 2;

	temp_8 = rand() % 1;
	memcpy(&msg[size], &temp_8, 1);//是否插入u盘
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

short usbFileMsg(char * msg)//进程信息报文
{
	FILE *fp = fopen("usbfiles.dat", "r");
	if (fp == NULL)
	{
		printf("usbfiles.dat打开失败");
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
	memcpy(&msg[size], &temp_16, 2);//报文的标识
	size += 2;
	temp_16 = htons(8 + num + 1);
	memcpy(&msg[size], &temp_16, 2);//报文总长度，其中八字节的报头，num字节的数据加一字节的尾零
	size += 2;
	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//2字节的填充0000
	size += 2;
	temp_16 = htons(num + 1);
	memcpy(&msg[size], &temp_16, 2);//数据长度，包括num个数据和一个尾零
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
	memcpy(&msg[size], &temp_16, 2);//报文格式标识
	size += 2;

	temp_16 = htons(44);
	memcpy(&msg[size], &temp_16, 2);//报文长度，固定为32+4+8
	size += 2;

	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//填充字节
	size += 2;

	temp_16 = htons(36);
	memcpy(&msg[size], &temp_16, 2);//数据段总长度
	size += 2;

	char on_off = rand() % 1;
	memcpy(&msg[size], &on_off, 1);//服务是否启动
	size += 1;

	temp_8 = rand();
	memcpy(&msg[size], &temp_8, 1);//pad
	size += 1;

	if (on_off == 1)
		temp_16 = htons(rand() % 26);
	else
		temp_16 = 0;
	memcpy(&msg[size], &temp_16, 2);//打印队列中现有任务数
	size += 2;

	int i;
	for (i = 0;i < 32;i++) 
	{
		temp_8 = rand() % ('z' - 'a') + 'a';
		memcpy(&msg[size], &temp_8, 1);//打印机名称
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
	memcpy(&msg[size], &temp_16, 2);//报文格式标识
	size += 2;

	temp_16 = htons(9);
	memcpy(&msg[size], &temp_16, 2);//报文长度，固定为9
	size += 2;

	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//填充字节
	size += 2;

	temp_16 = htons(1);
	memcpy(&msg[size], &temp_16, 2);//数据段总长度 固定为1
	size += 2;

	temp_8 = 0;
	memcpy(&msg[size], &temp_8, 1);//打印队列
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
	memcpy(&msg[size], &temp_16, 2);//报文格式标识
	size += 2;

	//temp_16 = htons(9);
	//memcpy(&msg[size], &temp_16, 2);//报文长度
	size += 2;

	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//填充字节
	size += 2;

	//temp_16 = htons(1);
	//memcpy(&msg[size], &temp_16, 2);//数据段总长度
	size += 2;


	char value[20];
	char errmsg[100];
	int min_dev_num = 5;//最小配置终端数量，缺省为5
	int max_dev_num = 28;//最大配置终端数量，缺省为28
	if (readconf("ts.log", "最小配置终端数量", value, errmsg) != -1)
		min_dev_num = atoi(value);
	if (readconf("ts.log", "最大配置终端数量", value, errmsg) != -1)
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
	for (i = 0;i < 16;i++)//填充哑终端
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
	memcpy(&msg[2], &temp_16, 2);//报文总长度
	temp_16 = htons(size-8);
	memcpy(&msg[6], &temp_16, 2);//报文数据总长度


	return size;
}

void virtualScreen(int id, int port, int ip, char *msg, short *size)
{
	char temp_8;
	short temp_16;
	int temp_32;

	temp_8 = id;
	memcpy(&msg[*size], &temp_8, 1);//虚屏编号
	*size += 1;

	temp_8 = rand();
	memcpy(&msg[*size], &temp_8, 1);//pad
	*size += 1;

	temp_16 = htons(port);
	memcpy(&msg[*size], &temp_16, 2);//远端服务器端口号
	*size += 2;

	temp_32 = htonl(ip);
	memcpy(&msg[*size], &temp_32, 4);//远端服务器端口号
	*size += 4;


	int sig = rand() % 2;
	char virtualProtocol[12];
	if (sig == 0)
		strcpy(virtualProtocol, "SSH");
	else
		strcpy(virtualProtocol, "专用SSH");
	memcpy(&msg[*size], virtualProtocol, 12);//虚屏协议
	*size += 12;

	sig = rand() % 3;
	char virtualStatus[8];
	if (sig == 0)
		strcpy(virtualStatus, "开机");
	else if (sig == 1)
		strcpy(virtualStatus, "关机");
	else
		strcpy(virtualStatus, "已登录");
	memcpy(&msg[*size], virtualProtocol, 8);//虚屏状态
	*size += 8;

	sig = rand() % 2;
	char virtualStr[24];
	if (sig == 0)
		strcpy(virtualStr, "储蓄系统");
	else
		strcpy(virtualStr, "基金开户");
	memcpy(&msg[*size], virtualStr, 24);//虚屏提示串
	*size += 24;


	sig = rand() % 2;
	char virtualType[12];
	if (sig == 0)
		strcpy(virtualStr, "vt100");
	else
		strcpy(virtualStr, "vt220");
	memcpy(&msg[*size], virtualStr, 12);//虚屏对应的终端类型
	*size += 12;

	int time_n = time();
	temp_32 = htonl(time_n);
	memcpy(&msg[*size], &temp_32, 4);//终端连接的时间
	*size += 4;


	//四个统计信息
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


	//三个ping值
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
	memcpy(&msg[size], &temp_8, 1);//报文格式标识
	size += 1;

	temp_8 = sig;
	memcpy(&msg[size], &temp_8, 1);//报文格式标识
	size += 1;

	//temp_16 = htons(9);
	//memcpy(&msg[size], &temp_16, 2);//报文长度
	size += 2;

	temp_16 = idTerminal;
	memcpy(&msg[size], &temp_16, 2);//填充字节
	size += 2;
	
	//temp_16 = htons(1);
	//memcpy(&msg[size], &temp_16, 2);//数据段总长度
	size += 2;

	//temp_8 = rand();
	temp_8 = ntohs(idTerminal);
	memcpy(&msg[size], &temp_8, 1);//端口号
	size += 1;

	//temp_8 = rand();
	temp_8 = ntohs(idTerminal);
	memcpy(&msg[size], &temp_8, 1);//配置端口号
	size += 1;

	char value[20];
	char errmsg[100];
	int min_tty_num = 3;//每个终端最小虚屏数量，缺省为3
	int max_tty_num = 10;//每个终端最大虚屏数量，缺省为10
	if (readconf("ts.log", "每个终端最小虚屏数量", value, errmsg) != -1)
		min_tty_num = atoi(value);
	if (readconf("ts.log", "每个终端最大虚屏数量", value, errmsg) != -1)
		max_tty_num = atoi(value);
	if (min_tty_num < 1 || min_tty_num>3)
		min_tty_num = 3;
	if (max_tty_num < 4 || max_tty_num>16)
		max_tty_num = 10;

	int screen_num = (rand() % (max_tty_num - min_tty_num)) + min_tty_num;
	//int screen_num = (rand() % max_tty_num) + min_tty_num;

	temp_8 = rand() % screen_num;
	memcpy(&msg[size], &temp_8, 1);//当前活动虚屏
	size += 1;

	*num_scr = screen_num;
	temp_8 = screen_num;
	memcpy(&msg[size], &temp_8, 1);//虚屏总量
	size += 1;

	if (sig == 0x0a)
		temp_32 = 0x00;
	else
		temp_32 = rand();

	memcpy(&msg[size], &temp_32, 4);//终端的ip地址
	size += 4;

	char typeTerminal[12];
	if (sig == 0x0a)
		strcpy(typeTerminal, "串口终端");
	else if (rand() % 1 == 0)
		strcpy(typeTerminal, "IP代理");
	else
		strcpy(typeTerminal, "IP终端");

	memcpy(&msg[size], typeTerminal, 12);//终端类型
	size += 12;

	char statusTerminal[8];
	if (rand() % 1 == 0)
		strcpy(statusTerminal, "正常");
	else
		strcpy(statusTerminal, "菜单");

	memcpy(&msg[size], statusTerminal, 8);//终端状态
	size += 8;

	int i;
	for (i = 0;i < screen_num;i++)
	{
		virtualScreen(i + 1, port, ip, msg, &size);
	}

	temp_16 = htons(size);
	memcpy(&msg[2], &temp_16, 2);//报文总长度
	temp_16 = htons(size - 8);
	memcpy(&msg[6], &temp_16, 2);//报文数据总长度

	return size;
}

short endMsg(char *msg)
{
	char temp_8;
	short temp_16;
	int temp_32;

	short size = 0;
	temp_16 = 0xff91;
	memcpy(&msg[size], &temp_16, 2);//报文格式标识
	size += 2;

	temp_16 = htons(8);
	memcpy(&msg[size], &temp_16, 2);//报文长度，固定为8
	size += 2;

	temp_16 = 0x0000;
	memcpy(&msg[size], &temp_16, 2);//填充字节
	size += 2;

	temp_16 = htons(0);
	memcpy(&msg[size], &temp_16, 2);//数据段总长度 固定为0
	size += 2;

}