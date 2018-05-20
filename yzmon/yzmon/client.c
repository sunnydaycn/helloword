      #include "client.h"

#define TEST 1

int finish = 0;
int connect_num = 0;

int tty_num = 0;
int scr_num = 0;



void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	while (1)
	{
		pid = waitpid(-1, &stat, WNOHANG);

		if (pid == 0)
			break;
		else if (pid < 0)
		{
			break;
		}
		else if (pid > 0) {
			//if (stat == 0)
			//{
			finish++;
			//writelog("ts.log", pid, "�������");
			//}
			//else
			//	writelog("ts.log", pid, "����ʧ��");
			connect_num--;
			printf("finish:%d\n", finish);
			printf("connect:%d\n", connect_num);
			//printf("child %d exit\n", pid);

		}
	}
}

void send_and_recv(struct sockaddr_in server_addr, int devid, int quit_or_not, int port, int ip)
{

	//���ӷ�����
	int socket_fd;
	int  max_socket;
	fd_set connectset;

	int ret;
	Connect:socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_fd == -1)
	{
		printf("socket error: % (errno:%d)", strerror(errno), errno);
		//connect_num--;
		exit(-1);
	}

	//��������socketΪ������                                                                      
	int flags = fcntl(socket_fd, F_GETFL, 0);
	fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);

	//���ӷ����

	FD_ZERO(&connectset);
	FD_SET(socket_fd, &connectset);
	ret = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
	while (select(socket_fd + 1, NULL, &connectset, NULL, NULL) <= 0)
		;
	ret = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
	//writelog("ts.log", getpid(), "���ӳɹ�");
	if (ret == -1)
	{
		printf("connect error: %s (errno:%d)\n", strerror(errno), errno);
		close(socket_fd);//////////////////////////////////////////
		exit(-1);
	}




	char buff[BUFF_SIZE];
	char Msg[BUFF_SIZE];
	int msg_len;
	int yibukou;//�첽������
	short Ethid;
	char typeOfVir;
	short idTer;
	unsigned char temp = 0;
	int i;
	char msgPart[8];

	char logName[30];
	sprintf(logName, "ts-%d.log", devid);
	//sprintf(logName, "");
	writelog(logName, devid, "fock�ӳ���ɹ�");

	//printf("�ӽ���\n");
	fd_set readset, writeset, bk_set;
	FD_ZERO(&bk_set);
	FD_SET(socket_fd, &bk_set);

	int retryTime = 0;
	int reconnectTime = 0;

	while (1)
	{
		writeset = readset = bk_set;
		if (select(socket_fd + 1, &readset, NULL, NULL, NULL))
		{
			if (FD_ISSET(socket_fd, &readset))
			{
				int n = 0;
				memset(buff, '\0', BUFF_SIZE);
				n = recv(socket_fd, buff, BUFF_SIZE, 0);
				if (n == 0)
				{
					if (temp == 0xff)//recv����0�������ϴν��ܵ��ı���ǰ�����ֽ���0x11ff��ʱ��Ͽ�����
					{
						//printf("devid: %d\n\n", devid);

						FILE *fp = fopen("1552144_ts_count.xls", "a+");
						if (fp == NULL)
						{
							printf("ts_count.xls��ʧ�ܡ�\n");
						}
						else
						{

							//��ȡ��ǰʱ��
							char time_s[9];
							time_t rawtime;
							struct tm *ptminfo;
							time(&rawtime);
							ptminfo = localtime(&rawtime);
							sprintf(time_s, "%02d:%02d:%02d", ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);



							fprintf(fp, "%s \t %d \t 1 \t %d \t %d \t \n", time_s, devid, tty_num, scr_num);
							fclose(fp);
						}
						tty_num = 0;
						scr_num = 0;
						close(socket_fd);
						exit(0);
					}
					else
					{
						printf("����˶Ͽ�   %2x\n", temp);
						//getchar();
						close(socket_fd);
						//kill(getppid(), SIGRTMIN);
						//connect_num--;
						goto Connect;
						//exit(-1);
					}
				}
				else if (n < 0)
					continue;
				char log[100];


				sprintf(log, "�������ݳɹ�����%d�ֽڡ�", n);
				writelog(logName, devid, log);
				writeDataInHex(logName, buff, n);


				memcpy(&temp, &buff[1], 1);
				if (temp == 0x01)
				{

					if (getServerTime(buff) == -1)
					{
						printf("����֤�����\n");
						close(socket_fd);
						//kill(getppid(), SIGRTMIN);
						//connect_num--;
						goto Connect;
						//exit(-1);
					}
					if (checkMsg(buff) == -1)
					{
						printf("��֤������\n");
						//kill(getppid(), SIGRTMIN);
						//connect_num--;
						close(socket_fd);
						goto Connect;
						//exit(-1);
					}

					retryTime = getRetryTime(buff);
					reconnectTime = getReconnectTime(buff);
					if (getServerVersion(buff) == -1)//������Ͱ汾����
					{
						sprintf(log, "������Ͱ汾����");
						writelog(logName, devid, log);

						msg_len = lowestVersion(Msg);


						sprintf(log, "��������%d�ֽڡ�", msg_len);
						writelog(logName, devid, log);
						writeDataInHex(logName, Msg, msg_len);
						writeset = readset = bk_set;
						if (select(socket_fd + 1, NULL, &writeset, NULL, NULL))
						{
							while ((send(socket_fd, Msg, msg_len, 0)) < 0)
							{
								if (errno != EAGAIN)
								{
									close(socket_fd);
									//kill(getppid(), SIGRTMIN);
									//connect_num--;
									printf("send error exit\n");
									goto Connect;
									//exit(-1);
								}
							}
						}
					}

					sprintf(log, "������֤����");
					writelog(logName, devid, log);

					msg_len = confBaseMsg(devid, Msg, &yibukou);

					sprintf(log, "��������%d�ֽڡ�", msg_len);
					writelog(logName, devid, log);
					writeDataInHex(logName, Msg, msg_len);

					writeset = readset = bk_set;
					if (select(socket_fd + 1, NULL, &writeset, NULL, NULL))//���ͱ���
					{
						while ((send(socket_fd, Msg, msg_len, 0)) < 0)
						{
							if (errno != EAGAIN)
							{
								close(socket_fd);
								//kill(getppid(), SIGRTMIN);
								//connect_num--;
								printf("send error exit\n");
								goto Connect;
								//exit(-1);
							}
						}
					}//if(select)


					//printf("%s\n", Msg);
				}//case 0x01

				else if (temp == 0x02)
				{
					for (i = 0;i < n;i += 8)
					{


						memcpy(msgPart, &buff[i], 8);
						memcpy(&temp, &msgPart[1], 1);
						if (temp == 0x02)
						{
							//printf("����ϵͳ����");
							sprintf(log, "����ϵͳ����");
							writelog(logName, devid, log);
							msg_len = sysMsg(Msg);

						}
						else if (temp == 0x03)
						{
							//printf("�������ñ���");
							sprintf(log, "�������ñ���");
							writelog(logName, devid, log);
							msg_len = confMsg(Msg);
						}//case 0x03:

						else if (temp == 0x04)
						{
							//printf("���ͽ��̱���");
							sprintf(log, "���ͽ��̱���");
							writelog(logName, devid, log);
							msg_len = proMsg(Msg);
						}//case 0x04:

						else if (temp == 0x05)
						{
							//printf("��������˿ڱ���");
							sprintf(log, "��������˿ڱ���");
							writelog(logName, devid, log);
							memcpy(&Ethid, &msgPart[4], 2);
							Ethid = ntohs(Ethid);
							msg_len = EthernetMsg(Ethid, Msg);
						}//case 0x05:

						else if (temp == 0x07)
						{
							//printf("����usb���ݱ���");
							sprintf(log, "����usb���ݱ���");
							writelog(logName, devid, log);
							msg_len = usbMsg(Msg);
						}//case 0x07:

						else if (temp == 0x08)
						{
							//printf("���ʹ�ӡ������");
							sprintf(log, "���ʹ�ӡ����Ϣ����");
							writelog(logName, devid, log);
							msg_len = printfMsg(Msg);
						}//case 0x08:

						else if (temp == 0x09)
						{
							//printf("���ͷ�����Ϣ����");
							sprintf(log, "���ͷ�����Ϣ����");
							writelog(logName, devid, log);

							msg_len = serverMsg(yibukou, Msg);
						}//case 0x09:

						sprintf(log, "��������%d�ֽڡ�", msg_len);
						writelog(logName, devid, log);
						writeDataInHex(logName, Msg, msg_len);
						writeset = readset = bk_set;
						if (select(socket_fd + 1, NULL, &writeset, NULL, NULL))
						{
							while ((send(socket_fd, Msg, msg_len, 0)) < 0)
							{
								if (errno != EAGAIN)
								{
									close(socket_fd);
									//kill(getppid(), SIGRTMIN);
									//connect_num--;
									printf("send error exit\n");
									goto Connect;
									//exit(-1);
								}
							}
						}
					}
				}//case 0x02:

				else if (temp == 0x0c)
				{
					//printf("����usb�ļ�����");
					sprintf(log, "����usb�ļ�����");
					writelog(logName, devid, log);
					msg_len = usbFileMsg(Msg);
				}//case 0x0c:


				else if (temp == 0x0d)
				{
					//printf("���ʹ�ӡ�б���");
					sprintf(log, "���ʹ�ӡ�б���");
					writelog(logName, devid, log);
					msg_len = prinfListMsg(Msg);
				}//case 0x0d:


				else if (temp == 0x0a || temp == 0x0b) {
					tty_num = n / 8;
					int num_scr;
					for (i = 0;i < n;i += 8)
					{
						memcpy(msgPart, &buff[i], 8);
						memcpy(&typeOfVir, &msgPart[1], 1);
						memcpy(&idTer, &msgPart[4], 2);
						if (typeOfVir == 0x0a)
						{
							//printf("����������Ϣ����(�ƶ˿�)");
							sprintf(log, "����������Ϣ����(�ƶ˿�)");
						}
						else
						{
							//printf("����������Ϣ����(IP�˿�)");
							sprintf(log, "����������Ϣ����(IP�˿�)");
						}
						writelog(logName, devid, log);
						msg_len = virtualScreenMsg(typeOfVir, idTer, port, ip, Msg, &num_scr);

						scr_num += num_scr;
						sprintf(log, "��������%d�ֽڡ�", msg_len);
						writelog(logName, devid, log);
						writeDataInHex(logName, Msg, msg_len);

						writeset = readset = bk_set;
						if (select(socket_fd + 1, NULL, &writeset, NULL, NULL))//���ͱ���
						{
							while ((send(socket_fd, Msg, msg_len, 0)) < 0)
							{
								if (errno != EAGAIN)
								{
									close(socket_fd);
									//kill(getppid(), SIGRTMIN);
									//connect_num--;
									printf("send error exit\n");
									goto Connect;
									//exit(-1);
								}
							}
						}//if(select)

					}
				}//case 0x0a:case 0x0b:

				else if (temp == 0xff)
				{
					//printf("���ͽ�������");
					sprintf(log, "���ͽ�������");
					writelog(logName, devid, log);
					msg_len = endMsg(Msg);

					sprintf(log, "��������%d�ֽڡ�", msg_len);
					writelog(logName, devid, log);
					writeDataInHex(logName, Msg, msg_len);

					writeset = readset = bk_set;
					if (select(socket_fd + 1, NULL, &writeset, NULL, NULL))//���ͱ���
					{
						while ((send(socket_fd, Msg, msg_len, 0)) < 0)
						{
							if (errno != EAGAIN)
							{
								close(socket_fd);
								//kill(getppid(), SIGRTMIN);
								//connect_num--;
								printf("send error exit\n");
								goto Connect;
								//exit(-1);
							}
						}
					}

				}
			}
		}
	}
	//return 0;
	exit(0);

}


void tcp_fork(int quit_or_not, int star_devid, int dev_num, struct sockaddr_in server_addr, int port, int ip)
{

	//�����ӽ����˳�����
	signal(SIGCHLD, &sig_chld);

	int pid;
	int i;
	int temp_i = 0;

	while (1) {
		if (finish >= dev_num)
		{
			printf("finish all 2 \n");
			break;
		}
		while (finish < (dev_num - 150) && connect_num > 150)
			;
		if ((connect_num + finish) < dev_num)
		{
			temp_i++;
			pid = fork();
			if (pid < 0)
			{
				printf("fork error\n");
				exit(-1);
			}
			else if (pid > 0)
			{
				connect_num++;
				continue;
			}
			else
			{
				//printf("temp_i:%d\n", temp_i);
				srand(time(0));//�������������
				send_and_recv(server_addr, star_devid + temp_i, quit_or_not, port, ip);
			}
		}
	}
	return;

}
int main(int argc, char *argv[])
{
	int star_devid = 0;
	int dev_num = 0;
	//������������
	star_devid = atoi(argv[1]);
	dev_num = atoi(argv[2]);



	//��ȡ�����ļ�����
	//�����������ʼ������
	char server_ip[15];//������ip��ַ
	strcpy(server_ip, "");
	int server_port = -1;//�������˿ں�
	int quit_or_not = 1;//���̽��ܳɹ����Ƿ��˳���1Ϊ�˳���0Ϊ���˳�������ʱ����ٴη������ݣ��Լ�ȱʡΪ1
	int deletelog_or_not = 1;//�Ƿ�ɾ���ϴε���־�ļ���1Ϊɾ����0Ϊ��ɾ��
	char DEBUGset[6];//debug����
	strcpy(DEBUGset, "000000");
	int DEBUG_p = 0;//�Ƿ���־�ļ���ʾ����Ļ��

					//��ȡ�ļ�
	char value[20];
	char errmsg[100];

	if (readconf("ts.conf", "������IP��ַ", value, errmsg) != -1)
		strcpy(server_ip, value);
	else
		printf("%s\n", errmsg);

	if (readconf("ts.conf", "�˿ں�", value, errmsg) != -1)
		server_port = atoi(value);
	else
		printf("%s\n", errmsg);

	if (readconf("ts.conf", "���̽��ճɹ����˳�", value, errmsg) != -1)
		quit_or_not = atoi(value);
	else
		printf("%s\n", errmsg);

	if (readconf("ts.conf", "ɾ����־�ļ�", value, errmsg) != -1)
		deletelog_or_not = atoi(value);
	else
		printf("%s\n", errmsg);

	if (readconf("ts.conf", "DEBUG����", value, errmsg) != -1)
		strcpy(DEBUGset, value);
	else
		printf("%s\n", errmsg);

	if (readconf("ts.conf", "DEBUG��Ļ��ʾ", value, errmsg) != -1)
		DEBUG_p = atoi(value);
	else
		printf("%s\n", errmsg);

	//������������
	if (quit_or_not > 1 || quit_or_not < 0)
		quit_or_not = 1;
	if (deletelog_or_not > 1 || deletelog_or_not < 0)
		deletelog_or_not = 1;
	if (strlen(DEBUGset) == 0)
		strcpy(DEBUGset, "000000");
	if (DEBUG_p > 1 || DEBUG_p < 0)
		DEBUG_p = 0;

	//if (deletelog_or_not == 1)
	//	remove("ts.log");

	struct sockaddr_in server_addr;
	//����Ҫ�󶨵�ip�Ͷ˿�
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(server_ip);
	server_addr.sin_port = htons(server_port);
	tcp_fork(quit_or_not, star_devid, dev_num, server_addr, server_port, inet_addr(server_ip));
	return 0;
}