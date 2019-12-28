// client.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <time.h>
#include <string>
#include <winsock2.h>
#include <iostream>
using namespace std;

#pragma warning(disable : 4996)
#pragma comment(lib, "Ws2_32.lib")

const string deault_ip = "127.0.0.1";
const int default_port = 1234;

string TimeStamp()
{
	time_t curr_time = time(nullptr);
	string ret = ctime(&curr_time);
	ret.pop_back();
	ret += " : ";
	return ret;
}

int main(int argc, char** argv)
{
	int error = 0;
	string ip = deault_ip;
	int port = default_port;
	bool reuseSocket = true;

	if (argc > 1 && argv[1] && strcmp(argv[1], "localhost") != 0) ip = argv[1];
	if (argc > 2 && argv[2]) port = atoi(argv[2]);

	cout << TimeStamp() << "Client will be sending on " << ip << ":" << port << endl;

	// initialize the use of Winsock DLL
	WSADATA wsaData;
	if ((error = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		cout << TimeStamp() << "Failed to initialize Winsock with error = " << error << endl;
		return error;
	}

	cout << TimeStamp() << "Initialized Winsock" << endl;

	SOCKET sendSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (sendSocket == INVALID_SOCKET) {
		error = WSAGetLastError();
		cout << TimeStamp() << "socket create failed with error = " << error << endl;
		WSACleanup();
		return error;
	}

	cout << TimeStamp() << "socket created" << endl;

	struct sockaddr_in serverInfo;
	int len = sizeof(serverInfo);
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons(port);
	serverInfo.sin_addr.s_addr = inet_addr(ip.c_str());

	while (true) {

		char buffer[512] = {0};
		ZeroMemory(buffer, sizeof(buffer));

		cout << endl << "Please input your message: ";
		cin.getline(buffer, sizeof(buffer));

		int bytes_to_send = sizeof(buffer);
		char * lpbuf = buffer;

		while (bytes_to_send) {
			int len = sendto(sendSocket, buffer, sizeof(buffer), 0, (sockaddr*)&serverInfo, sizeof(serverInfo));
			if (len == 0 || len == SOCKET_ERROR) break;

			cout << TimeStamp() << "Sent : Size = " << len << " Message = " << buffer << endl << endl;

			bytes_to_send -= len;
			lpbuf += len;
		}
	}

	closesocket(sendSocket);
	WSACleanup();
	return 0;
}