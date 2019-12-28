#include <time.h>
#include <string>
#include <winsock2.h>
#include <iostream>
using namespace std;

#pragma warning(disable : 4996)
#pragma comment(lib, "Ws2_32.lib")

const int port = 20000;
const int sendtimeout = 300;
const int recvtimeout = 300;

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
	int         error = 0;
	char        broadcast = '1';
	WSADATA     wsaData;
	SOCKET      clientsocket;
	struct  sockaddr_in sendaddr;
	struct  sockaddr_in recvaddr;


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

	if (setsockopt(clientsocket, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(broadcast)) == SOCKET_ERROR) {
		error = WSAGetLastError();
		cout << TimeStamp() << "Failed to set SO_BROADCAST = " << error << endl;
		closesocket(clientsocket);
		WSACleanup();
		return error;
	}

	setsockopt(clientsocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&sendtimeout, sizeof(sendtimeout));
	setsockopt(clientsocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvtimeout, sizeof(recvtimeout));

	sendaddr.sin_family = AF_INET;
	sendaddr.sin_port = htons(port);
	sendaddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

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
