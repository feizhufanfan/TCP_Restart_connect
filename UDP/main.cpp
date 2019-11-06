#include<WinSock2.h>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <queue>
#pragma comment(lib,"ws2_32.lib")
typedef struct {
	unsigned char 	Head; 						//帧头  			值见宏值定义
	unsigned short	packetID;					//消息ID			值见宏值定义
	unsigned char 	frameID;					//帧号				(0x00 ~ 0xFF)
	short 			sw_angle; 					//方向盘角度		(-8000 ~ 8000) 单位0.1°
	unsigned char	speed;						//车速				单位km/h
	unsigned char 	xor_verify;  				//异或校验			(对结构体中中除了帧头帧尾以外的成员进行异或校验)
	unsigned char	Tail;						//帧尾				值见宏值定义
}STEERING_WHEEL_UDP;
std::queue<STEERING_WHEEL_UDP> queue_data;

void data_revice() {
	char *cech_data = new char[sizeof(STEERING_WHEEL_UDP)];
	std::cout << "thread start!";
	WSADATA wsd;    // 初始化Socket的变量
	SOCKET s;        // 用于通信的Socket句柄
	SOCKADDR_IN sRecvAddr, sSendAddr;    // 分别为接收地址和发送地址
	USHORT uPort = 1401;                // 通信端口
    char szBuf[sizeof(STEERING_WHEEL_UDP)] = { 0 };            // 通信数据缓冲区
   int nBufLen = 1024, nResult = 0, nSenderAddrSize = sizeof(sSendAddr);

   // 初始化Socket2.2版本
    nResult = WSAStartup(MAKEWORD(2, 2), &wsd);
    if (nResult != NO_ERROR)
   {
       printf("WSAStartup failed:%d\n", WSAGetLastError());
        return ;
    }

		     // 创建一个Socket,SOCK_DGRAM表示UDP类型
		     s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET)
   {
       printf("socket failed:%d\n", WSAGetLastError());
       return ;
    }

  // 填充Socket接口
	sRecvAddr.sin_family = AF_INET;        // 地址协议,AF_INET支持TCP和UDP
	sRecvAddr.sin_port = htons(uPort);    // 通信端口,htons转为网络字节顺序
	sRecvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);    // 接收任意地址数据

    // 绑定Socket至本机
    nResult = bind(s, (SOCKADDR *)&sRecvAddr, sizeof(sRecvAddr));
    if (nResult != 0)
   {
     printf("bind failed:%d\n", WSAGetLastError());
      return ;
   }

   printf("Waiting recv data...\n");
   // 阻塞式接收数据
   while (1) {
	   //std::cout << "thread 1!";

	   int len = recvfrom(s, szBuf, sizeof(STEERING_WHEEL_UDP), 0, (SOCKADDR *)&sSendAddr, &nSenderAddrSize);
	   //std::cout << "thread 2!";
	   if (len > 0) {
		  // std::cout <<"thread data !"<< " ";
		  // std::cout << szBuf[0] << " ";
		   if ((szBuf[0] == (char)0xca) && (szBuf[8] ==(char)0xac)) {
			  // std::cout << " data jiexi!" << " ";
			   STEERING_WHEEL_UDP data;
			   data.Head = 0xCA;
			   data.packetID = (0xFFFF & (szBuf[1] << 8))& szBuf[2];
			   data.frameID = szBuf[3];
			   data.sw_angle = (0xFFFF & (szBuf[4] << 8))& szBuf[5];
			   data.speed = szBuf[6];
			   data.xor_verify = szBuf[7];
			   data.Tail = 0xAC;
			   //if()
			  // std::cout << std::hex << szBuf[6];
			   queue_data.push(data);
		   }

	   }
   }
  // nResult = recvfrom(s, szBuf, nBufLen, 0, (SOCKADDR *)&sSendAddr, &nSenderAddrSize);
   if (nResult == SOCKET_ERROR)
   {
	   printf("recvfrom failed:%d\n", WSAGetLastError());
   }else {
      printf("SenderIP  :%s\n", inet_ntoa(sSendAddr.sin_addr));
      printf("SenderData:%s\n", szBuf);

	}
    // 关闭Socket连接
   nResult = closesocket(s);
    if (nResult == SOCKET_ERROR)
  {
       printf("closesocket failed:%d\n", WSAGetLastError());
        return ;
   }
    // 清理Socket
    WSACleanup();

   // system("pause");
	    return ;
	
}
int main(int argc, char* argv[])
{
	//STEERING_WHEEL_UDP date;
	//data_test.Head =0 ;
	WSADATA WSAData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (WSAStartup(sockVersion, &WSAData) != 0)
		return 0;
	std::thread t_fun(data_revice);
	t_fun.detach();
	while (1) {
		while (!queue_data.empty())
		{
			//std::cout << "main Thread data!";
			std::cout <<queue_data.front().speed<<"\n";
			//std::cout << "pop data!";
			queue_data.pop();
		}
	};
	//
	//char buff[1024];	//建立接收缓存字节数组 
	//while (true)
	//{
	//	memset(buff, 0, 1024);	//清空接收缓存数组
	//							//开始接收数据 
	//	int len = recvfrom(serSocket, buff, 1024, 0, (sockaddr*)&clientAddr, &iAddrlen);
	//	if (len>0)
	//	{
	//		cout << "客户端地址：" << inet_ntoa(clientAddr.sin_addr) << endl;
	//		cout << buff;

	//		//	sendto(serSocket,buff,1024,0,(sockaddr*)&clientAddr,iAddrlen);
	//	}
	//}

	
	WSACleanup();
	

	return 0;
}

