#include"rconf.h"

/*
参数说明
fileName:要读取的配置文件的文件名
parameter：要读取的参数的参数名
value：读取到的参数的值
errMsg：如果发生错误，返回错误信息
*/
int readconf(char * fileName,char * parameter,char *value,char * errMsg)
{
	FILE *fp;
	int i;
	fp=fopen(fileName,"r");
	if(fp==NULL)
	{
		if(errMsg!=NULL)
			strcpy(errMsg,"文件打开失败");
		return -1;
	}
	char temp[200];
	int sig=0;//标记位，0表示尚未找到对应参数行，1表示已经找到对应参数行
	while (!feof(fp))
	{
		fgets(temp,200,fp);
		//在查找参数是否存在的时候去除#后面的注释的影响
		char * p;
		p=strchr(temp,'#');
		if(p!=NULL)
			*p=0;
		if(strstr(temp,parameter)!=NULL){
			sig=1;
			break;
		}
	}
	if(sig==0)
	{
		if(errMsg!=NULL)
			strcpy(errMsg,"未在指定文件中找到指定参数行");
		return -1;
	}
	//制表符替换为空格
	for(i=0;temp[i]!=0;i++)
		if(temp[i]=='\t')
			temp[i]=' ';

	
	//多个空格和为一个
	char result[200];
	strcpy(result,temp);
	for (i = 0;temp[i + 1] != 0;i++)
	{
		if ((temp[i] == ' ') && (temp[i + 1] == ' ')) {
			strcpy(&result[i], &temp[i + 1]);
			i--;
		}
		strcpy(temp, result);
	}

	//去除头尾空格

	if(temp[0]==' '||temp[0]=='\t')
		strcpy(temp,&temp[1]);
	//此时的temp字符串应该为这样的：参数 value
	//读取参数值
	
	strcpy(value,temp+strlen(parameter)+1);//注意，在属性和值之间还有一个空格  要忽略这个空格
	//printf("%s\n", value);
	fclose(fp);
}



short readCpuHZ()
{
	char conf[] = "/proc/cpuinfo";
	char parameter[] = "cpu Mhz";
	char cpuHz_s[8];
	int cpuHz;
	if (readconf(conf, parameter, cpuHz_s, NULL) != -1)
	{
		cpuHz = atoi(&cpuHz_s[1]);//去除“：”号
		printf("cpuHz:%d\n", cpuHz);
		return cpuHz;
	}
	else
		return -1;
}

int readMemSize()
{
	char conf[] = "/proc/cpuinfo";
	char parameter[] = "MemTotal:";
	char memSize_s[8];
	int memSize;
	if (readconf(conf, parameter, memSize_s, NULL) != -1)
	{
		memSize = atoi(memSize_s);
		return memSize;
	}
	else
		return -1;
}

int readCpuTime(int *userTime, int *niceTime, int *systemTime, int *idleTime)
{
	FILE *fp;
	int i;
	char *fileName = "/proc/stat";
	fp = fopen(fileName, "r");
	if (fp == NULL)
	{
		printf("文件打开失败");
		return -1;
	}
	char temp[30];
	while (!feof(fp))
	{
		fscanf(fp, "%s", temp);
		if (strcmp(temp, "cpu"))
		{
			fscanf(fp, "%d", userTime);
			fscanf(fp, "%d", niceTime);
			fscanf(fp, "%d", systemTime);
			fscanf(fp, "%d", idleTime);
			return 0;
		}
	}
	return -1;
}

int readFreedMem()
{
	char conf[] = "/proc/cpuinfo";
	char parameter1[] = "MemFree:";
	char parameter2[] = "Buffers:";
	char parameter3[] = "Cached:";

	char memSize_s[8];
	int memSize;
	int memFreeSize = 0;
	if (readconf(conf, parameter1, memSize_s, NULL) != -1)
	{
		memSize = atoi(memSize_s);
		memFreeSize += memSize;
	}
	else
		return -1;
	if (readconf(conf, parameter2, memSize_s, NULL) != -1)
	{
		memSize = atoi(memSize_s);
		memFreeSize += memSize;
	}
	else
		return -1;
	if (readconf(conf, parameter3, memSize_s, NULL) != -1)
	{
		memSize = atoi(memSize_s);
		memFreeSize += memSize;
	}
	else
		return -1;


	return memFreeSize/1000;//单位化为MB
}