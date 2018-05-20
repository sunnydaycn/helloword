#include"wlog.h"

int writelog(char *logName, int num_s, char *data)
//int main()
{
	if (strlen(logName) == 0)
		return -1;
	//获取当前时间
	char time_s[9];
	time_t rawtime;
	struct tm *ptminfo;
	time(&rawtime);
	ptminfo = localtime(&rawtime);
	sprintf(time_s, "%02d:%02d:%02d", ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);

	// char logName[]="ts.log";
	// char num_s[]="155214401";
	// char data[]="test";
	FILE *fp;
	fp = fopen(logName, "a+");
	if (fp == NULL)
	{
		printf("日志文件打开失败");
		return -1;
	}
	//printf( "%s [%d] %s\n", time_s, num_s, data);
	fprintf(fp, "%s [%d] %s\n", time_s, num_s, data);
	fclose(fp);
	return 0;
}

int writeDataInHex(char *logName, char *data_i,int length)
{
	if (strlen(logName) == 0)
		return -1;

	unsigned char * data = (unsigned char *)data_i;
	int i, j;
	FILE *fp;
	fp = fopen(logName, "a+");
	if (fp == NULL)
	{
		printf("日志文件打开失败");
		return -1;
	}
	i = 0;
	//先处理满16个字符一行的数据
	while ((i + 16) < length)
	{
		//输出这一行的起始位数，以16进制输出，一行16个数字
		fprintf(fp, "%04x:  ", i);
		//输出16位字符对应的16进制数字
		for (j = i;j < i + 16;j++)
		{
			fprintf(fp, "%02x ", data[j]);
			if (j == (i + 7))
				fprintf(fp, " - ");
		}

		//输出16位字符对应的字符，无法显示的字符用“.”表示
		fprintf(fp, "  ");
		for (j = i;j < i + 16;j++)
		{
			if (data[j] > 31 && data[j] < 127)
				fprintf(fp, "%c", data[j]);
			else
				fprintf(fp, ".");
		}
		fprintf(fp, "\n");
		i += 16;
	}
	//余下未输出的字符
	if (i != length)
	{
		//i-=16;//若计数器和数据长度不一致，则为有未满16个一组的数据
		fprintf(fp, "%04x:  ", i);//起始位数不要忘记输出
		//输出16位字符对应的16进制数字，不足的用空格补齐
		for (j = i;j < i + 16;j++)
		{
			if (j < length)
			{
				if (j == (i + 8))
					fprintf(fp, " - ");
				fprintf(fp, "%02x ", data[j]);
			}
			else {
				fprintf(fp, "   ");
				if (j == (i + 8))
					fprintf(fp, "   ");
			}
		}
		//输出剩余字符对应的字符，无法显示的字符用“.”表示
		fprintf(fp, "  ");
		for (j = i;j < length;j++)
		{
			if (data[j] > 31 && data[j] < 127)
				fprintf(fp, "%c", data[j]);
			else
				fprintf(fp, ".");
		}
		fprintf(fp, "\n");
	}

	fclose(fp);
	
}