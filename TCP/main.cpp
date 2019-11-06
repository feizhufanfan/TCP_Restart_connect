#include <iostream>
#include <thread>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
//using namespace std;
void initialization();

typedef struct {
	unsigned char 	Head; 						//帧头  			值见宏值定义
	unsigned short	packetID;					//消息ID			值见宏值定义
	unsigned char 	frameID;					//帧号				(0x00 ~ 0xFF)
	short 			sw_angle; 					//方向盘角度		(-8000 ~ 8000) 单位0.1°
	unsigned char	speed;						//车速				单位km/h
	unsigned char 	xor_verify;  				//异或校验			(对结构体中中除了帧头帧尾以外的成员进行异或校验)
	unsigned char	Tail;						//帧尾				值见宏值定义
}STEERING_WHEEL_UDP;

int main() {
	//定义长度变量
	int send_len = 0;
	int recv_len = 0;
	int len = 0;
	//定义发送缓冲区和接受缓冲区
	char send_buf[100];
	char recv_buf[sizeof(STEERING_WHEEL_UDP)];
	//定义服务端套接字，接受请求套接字
	SOCKET s_server;
	SOCKET s_accept;
	//服务端地址客户端地址
	SOCKADDR_IN server_addr;
	SOCKADDR_IN accept_addr;
	initialization();
	//填充服务端信息
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(5030);
	//创建套接字
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		std::cout << "套接字绑定失败！" << std::endl;
		WSACleanup();
	}
	else {
		std::cout << "套接字绑定成功！" << std::endl;
	}
	//设置套接字为监听状态
	if (listen(s_server, SOMAXCONN) < 0) {
		std::cout << "设置监听状态失败！" << std::endl;
		WSACleanup();
	}
	else {
		std::cout << "设置监听状态成功！" << std::endl;
	}
	std::cout << "服务端正在监听连接，请稍候...." << std::endl;
	//接受连接请求
	len = sizeof(SOCKADDR);
	s_accept = accept(s_server, (SOCKADDR *)&accept_addr, &len);
	if (s_accept == SOCKET_ERROR) {
		std::cout << "连接失败！" << std::endl;
		WSACleanup();
		return 0;
	}
	std::cout << "连接建立，准备接受数据" << std::endl;


	//setsockopt(,);
	//接收数据
	while (1) {
		recv_len = recv(s_server, recv_buf, sizeof(STEERING_WHEEL_UDP), 0);
		if (recv_len < 0) {
			//std::cout << "接受失败！" << std::endl;
			//break;
		}
		else {
			//int *p = &recv_buf;
			std::cout << "客户端信息:" << (int)recv_buf << std::endl;
		}
		/*std::cout << "请输入回复信息:";
		std::cin >> send_buf;
		send_len = send(s_accept, send_buf, sizeof(send_buf), 0);
		if (send_len < 0) {
			std::cout << "发送失败！" << std::endl;
			break;
		}*/
	}
	//关闭套接字
	closesocket(s_server);
	closesocket(s_accept);
	//释放DLL资源
	WSACleanup();
	return 0;
}
void initialization() {
	//初始化套接字库
	WORD w_req = MAKEWORD(2, 2);//版本号
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		std::cout << "初始化套接字库失败！" << std::endl;
	}
	else {
		std::cout << "初始化套接字库成功！" << std::endl;
	}
	//检测版本号
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		std::cout << "套接字库版本号不符！" << std::endl;
		WSACleanup();
	}
	else {
		std::cout << "套接字库版本正确！" << std::endl;
	}
	//填充服务端地址信息

}
