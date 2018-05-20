#include"wlog.h"

int writelog(char *logName, int num_s, char *data)
//int main()
{
	if (strlen(logName) == 0)
		return -1;
	//��ȡ��ǰʱ��
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
		printf("��־�ļ���ʧ��");
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
		printf("��־�ļ���ʧ��");
		return -1;
	}
	i = 0;
	//�ȴ�����16���ַ�һ�е�����
	while ((i + 16) < length)
	{
		//�����һ�е���ʼλ������16���������һ��16������
		fprintf(fp, "%04x:  ", i);
		//���16λ�ַ���Ӧ��16��������
		for (j = i;j < i + 16;j++)
		{
			fprintf(fp, "%02x ", data[j]);
			if (j == (i + 7))
				fprintf(fp, " - ");
		}

		//���16λ�ַ���Ӧ���ַ����޷���ʾ���ַ��á�.����ʾ
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
	//����δ������ַ�
	if (i != length)
	{
		//i-=16;//�������������ݳ��Ȳ�һ�£���Ϊ��δ��16��һ�������
		fprintf(fp, "%04x:  ", i);//��ʼλ����Ҫ�������
		//���16λ�ַ���Ӧ��16�������֣�������ÿո���
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
		//���ʣ���ַ���Ӧ���ַ����޷���ʾ���ַ��á�.����ʾ
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