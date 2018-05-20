#include"rconf.h"

/*
����˵��
fileName:Ҫ��ȡ�������ļ����ļ���
parameter��Ҫ��ȡ�Ĳ����Ĳ�����
value����ȡ���Ĳ�����ֵ
errMsg������������󣬷��ش�����Ϣ
*/
int readconf(char * fileName,char * parameter,char *value,char * errMsg)
{
	FILE *fp;
	int i;
	fp=fopen(fileName,"r");
	if(fp==NULL)
	{
		if(errMsg!=NULL)
			strcpy(errMsg,"�ļ���ʧ��");
		return -1;
	}
	char temp[200];
	int sig=0;//���λ��0��ʾ��δ�ҵ���Ӧ�����У�1��ʾ�Ѿ��ҵ���Ӧ������
	while (!feof(fp))
	{
		fgets(temp,200,fp);
		//�ڲ��Ҳ����Ƿ���ڵ�ʱ��ȥ��#�����ע�͵�Ӱ��
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
			strcpy(errMsg,"δ��ָ���ļ����ҵ�ָ��������");
		return -1;
	}
	//�Ʊ���滻Ϊ�ո�
	for(i=0;temp[i]!=0;i++)
		if(temp[i]=='\t')
			temp[i]=' ';

	
	//����ո��Ϊһ��
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

	//ȥ��ͷβ�ո�

	if(temp[0]==' '||temp[0]=='\t')
		strcpy(temp,&temp[1]);
	//��ʱ��temp�ַ���Ӧ��Ϊ�����ģ����� value
	//��ȡ����ֵ
	
	strcpy(value,temp+strlen(parameter)+1);//ע�⣬�����Ժ�ֵ֮�仹��һ���ո�  Ҫ��������ո�
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
		cpuHz = atoi(&cpuHz_s[1]);//ȥ����������
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
		printf("�ļ���ʧ��");
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


	return memFreeSize/1000;//��λ��ΪMB
}