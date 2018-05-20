#include<windows.h>
#include<iostream>
using namespace std;
#include "mmsystem.h"//导入声音头文件 
#pragma comment(lib,"winmm.lib")//导入声音头文件库 
#include<stdio.h>
void PlayMp3(char a[]);
void main()
{
	char a[] = "f:\\2.wav";
	PlayMp3(a);
	PlaySound("f:\\1.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	while(1)
	cout << 'a';
	system("pause");

}
void PlayMp3(char a[])
{
	char buf[128];
	char str[128] = { 0 };
	int i = 0;

	//use mciSendCommand 
	MCI_OPEN_PARMS mciOpen;
	MCIERROR mciError;
	SetWindowText(NULL,"12345"); 
	mciOpen.lpstrDeviceType = "mpegvideo";
	mciOpen.lpstrElementName = a;
	mciError = mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)&mciOpen);
	if (mciError)
	{
		mciGetErrorString(mciError, buf, 128);
		printf("send MCI_OPEN command failed:%s\n", buf);
		return;
	}
	UINT DeviceID = mciOpen.wDeviceID;
	MCI_PLAY_PARMS mciPlay;

	mciError = mciSendCommand(DeviceID, MCI_PLAY, 0, (DWORD)&mciPlay);
	if (mciError)
	{
		printf("send MCI_PLAY command failed\n");
		return;
	}
}