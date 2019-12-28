#include <time.h>
#include <string>
#include <winsock2.h>
#include <iostream>
using namespace std;

#pragma warning(disable : 4996)
#pragma comment(lib, "Ws2_32.lib")

const int port = 20000;

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
	SOCKET      serversocket;
	struct  sockaddr_in sendaddr;
	struct  sockaddr_in recvaddr;

	if ((error = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		cout << TimeStamp() << "Failed to initialize Winsock with error = " << error << endl;
		return error;
	}

	cout << TimeStamp() << "Initialized Winsock" << endl;

	serversocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serversocket == INVALID_SOCKET) {
		error = WSAGetLastError();
		cout << TimeStamp() << "socket create failed with error = " << error << endl;
		WSACleanup();
		return error;
	}

	cout << TimeStamp() << "socket created" << endl;

	if (setsockopt(serversocket, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(broadcast)) == SOCKET_ERROR) {
		error = WSAGetLastError();
		cout << TimeStamp() << "Failed to set SO_BROADCAST = " << error << endl;
		closesocket(serversocket);
		WSACleanup();
		return error;
	}

	setsockopt(serversocket, SOL_SOCKET, SO_REUSEADDR, (char*)&broadcast, sizeof(broadcast));

	sendaddr.sin_family = AF_INET;
	sendaddr.sin_port = htons(port);
	sendaddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(serversocket, (SOCKADDR*)&sendaddr, sizeof(sendaddr)) == SOCKET_ERROR) {
		error = WSAGetLastError();
		cout << TimeStamp() << "Failed to bind with error = " << error << endl;
		closesocket(serversocket);
		WSACleanup();
		return error;
	}

	while (true) {

		int recvlen = sizeof(recvaddr);
		char recvbuf[65535];
		ZeroMemory(recvbuf, sizeof(recvbuf));

		int ret = recvfrom(serversocket, recvbuf, sizeof(recvbuf), 0, (SOCKADDR*)&recvaddr, &recvlen);

		if (ret == SOCKET_ERROR) {
			error = WSAGetLastError();
			cout << TimeStamp() << "Failed to recv with error = " << error << endl;
			closesocket(serversocket);
			WSACleanup();
			return error;
		}

		cout << TimeStamp() << "Recv : Size = " << strlen(recvbuf) << " Message = " << recvbuf << endl;
	}

	closesocket(serversocket);
	WSACleanup();
	return error;
}
