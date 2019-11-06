#include <WinSock2.h>
#include <iostream>
#include <thread>
#pragma comment(lib,"ws2_32.lib")

using namespace std;

typedef struct {
	unsigned char 	Head; 						//帧头  			值见宏值定义
	unsigned short	packetID;					//消息ID			值见宏值定义
	unsigned char 	frameID;					//帧号				(0x00 ~ 0xFF)
	short 			sw_angle; 					//方向盘角度		(-8000 ~ 8000) 单位0.1°
	unsigned char	speed;						//车速				单位km/h
	unsigned char 	xor_verify;  				//异或校验			(对结构体中中除了帧头帧尾以外的成员进行异或校验)
	unsigned char	Tail;						//帧尾				值见宏值定义
}STEERING_WHEEL_UDP;

const int BUF_SIZE = sizeof(STEERING_WHEEL_UDP);
WSADATA         wsd;            //WSADATA变量  
SOCKET          sHost;          //服务器套接字  
SOCKADDR_IN     servAddr;       //服务器地址  
char            buf[BUF_SIZE];  //接收数据缓冲区  
char            bufRecv[BUF_SIZE];
int             retVal;         //返回值  



//BOOL RecvLine(SOCKET s, char* buf);  //读取一行数据  
int g_nRevThread_flag = 0;
int g_nrevThread_run_flag = 0;


/********************断线重连线程************************************/
void isConnect();
int out_time = 0;										//重连超时计数器


int main(int argc, char* argv[])
{

									//初始化套结字动态库  
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		cout  << "WSAStartup failed!" << endl;
		return  - 1;
	}

	//创建套接字  
	sHost  = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET  == sHost)
	{
		cout  << "socket failed!" << endl;
		WSACleanup();//释放套接字资源  
		return   - 1;
	}

	//设置服务器地址  
	servAddr.sin_family  = AF_INET;
	servAddr.sin_addr.s_addr  = inet_addr("192.168.1.171");
	servAddr.sin_port  = htons((short)6000);
	int nServAddlen   = sizeof(servAddr);

	//连接服务器  
	retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
	
	if (SOCKET_ERROR  == retVal)
	{
		cout  << "connect failed!" << endl;
		//closesocket(sHost); //关闭套接字  
		//WSACleanup();       //释放套接字资源  
		//return  - 1;
		g_nRevThread_flag = 0;
	}
	
	/********初始话自动重连线程***************/
	//std::thread p(isConnect, &sHost, &servAddr);
	
	while (true) {
		//向服务器发送数据  
		//ZeroMemory(buf, BUF_SIZE);
		/*cout  << " 向服务器发送数据:  ";
		cin  >> buf;*/
		//retVal  = send(sHost, buf, strlen(buf), 0);
		//if (SOCKET_ERROR  == retVal)
		//{
		//	cout  << "send failed!" << endl;
		//	closesocket(sHost); //关闭套接字  
		//	//WSACleanup();       //释放套接字资源  
		//	//return  - 1;
		//}
		//RecvLine(sHost, bufRecv);
		char cace_buf[1024] = {0};
		retVal = recv(sHost, cace_buf, 1024,0);
		if (retVal>0) {
		   // 接收服务器端的数据， 只接收5个字符  
			cout  << endl  << "从服务器接收数据：" << cace_buf;
			out_time = 0;
		}
		else {
			if (g_nrevThread_run_flag ==0&& out_time<60) {
				std::cout << "判断线程！g_nrevThread_run_flag:" << g_nrevThread_run_flag << endl;
				std::thread t_restart_connect(isConnect);
				t_restart_connect.detach();
			}
			/*****************长时间连接未响应自动结束程序*********************/
			if (out_time > 70) {
					closesocket(sHost); //关闭套接字  
					WSACleanup();       //释放套接字资源  
					std::cout << "结束重连线程！将在2后自动结束程序！" <<endl;
					Sleep(2000);
					return -1;
			}
			//g_nRevThread_flag = 1;
			///****************
			//g_nRevThread_flag: 1表示需要进行重连   0表示不需要进行重连（连接成功或者出现不可自动恢复性问题）
			//*************************/
			//if (g_nRevThread_flag) {
			//	/****************检测该线程是否已经运行***
			//	g_nrevThread_run_flag 1表示该线程已经运行 0表示该线程已经结束
			//	**************/
			//
			//	if (!t_restart_connect.joinable()) {
			//		std::cout << "开启重连！";
			//		t_restart_connect.detach();
			//		g_nrevThread_run_flag =0;
			//	}
			//	
			//}
			//else {
			//	if(g_nrevThread_run_flag)
			//		t_restart_connect.~thread();
			//	g_nrevThread_run_flag = 1;
			//	closesocket(sHost); //关闭套接字  
			//	WSACleanup();
			//	cout << "连接失败！"<<endl;
			//	}
		}
		

	}
	cout << "end main!";
	//退出  
	//t_restart_connect.~thread();
	closesocket(sHost); //关闭套接字  
	WSACleanup();       //释放套接字资源  
	system("pause");
	return 0;
}
//

void isConnect() {
	g_nrevThread_run_flag = 1;
	for (USHORT i = 1; i < 0x1000; i = i << 1) {
		out_time = i;
		if (sHost != NULL)
			closesocket(sHost); //关闭套接字
		sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == sHost)
		{
			cout << "socket failed!" << endl;
			//WSACleanup();//释放套接字资源  
			//return  -1;
			std::cout << "发起重连将在" << i << "s后开始！" << endl;
			Sleep(i*1000);
			
			continue;
		}
		int retVal;
		retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
		if (SOCKET_ERROR == retVal)
		{
			cout << "connect failed!" << endl;
			if (sHost != NULL)
				closesocket(sHost); //关闭套接字  
									//WSACleanup();       //释放套接字资源  
									//return  -1;
			std::cout << "发起重连将在" << i << "s后开始！" << endl;
			Sleep(i * 1000);
		
			continue;
		}
		/***********连接成功后设置标志位,不再尝试重连***********

		**************/
		g_nRevThread_flag = 0;
		g_nrevThread_run_flag = 0;
		std::cout << "离开重连线程！g_nrevThread_run_flag:"<< g_nrevThread_run_flag << endl;
		return;
	}
	std::cout << "离开重连线程！g_nrevThread_run_flag:" << g_nrevThread_run_flag << endl;
	g_nRevThread_flag = 0;
	g_nrevThread_run_flag = 0;
}