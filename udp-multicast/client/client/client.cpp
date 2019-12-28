#include <time.h>
#include <string>
#include <winsock2.h>
#include <iostream>
using namespace std;

#pragma warning(disable : 4996)
#pragma comment(lib, "Ws2_32.lib")

const int port = 20000;
const char MULTICAST_IP[] = "234.9.9.9";

string TimeStamp()
{
	time_t curr_time = time(nullptr);
	string ret = ctime(&curr_time);
	ret.pop_back();
	ret += " : ";
	return ret;
}


int main()
{
	int		error = 0;
	WSADATA wsaData;
	SOCKET	clientsocket;

	if ((error = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		cout << TimeStamp() << "Failed to initialize Winsock with error = " << error << endl;
		return error;
	}

	cout << TimeStamp() << "Initialized Winsock" << endl;

	clientsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (clientsocket == INVALID_SOCKET) {
		error = WSAGetLastError();
		cout << TimeStamp() << "socket create failed with error = " << error << endl;
		WSACleanup();
		return error;
	}

	cout << TimeStamp() << "socket created" << endl;

	struct sockaddr_in sendaddr;
	sendaddr.sin_family = AF_INET;
	sendaddr.sin_port = htons(port);
	sendaddr.sin_addr.s_addr = inet_addr(MULTICAST_IP);

	while (true) {

		char sendbuf[512] = { 0 };
		ZeroMemory(sendbuf, sizeof(sendbuf));

		cout << endl << "Enter input message : ";
		cin.getline(sendbuf, sizeof(sendbuf));

		int sendlen = sendto(clientsocket, sendbuf, strlen(sendbuf), 0, (sockaddr*)&sendaddr, sizeof(sendaddr));
		if (sendlen == SOCKET_ERROR) {
			error = WSAGetLastError();
			cout << TimeStamp() << "Failed to send with error = " << error << endl;
			closesocket(clientsocket);
			WSACleanup();
			return error;
		}

		cout << TimeStamp() << "Sent : Size = " << sendlen << " Message = " << sendbuf << endl;
	}

	closesocket(clientsocket);
	WSACleanup();
	return error;
}
