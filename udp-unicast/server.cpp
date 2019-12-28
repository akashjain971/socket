// server.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <time.h>
#include <string>
#include <winsock2.h>
#include <iostream>
using namespace std;

#pragma warning(disable : 4996)
#pragma comment(lib, "Ws2_32.lib")

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
	int port = default_port;

	if (argc > 1 && argv[1]) port = atoi(argv[1]);

	cout << TimeStamp() << "Server will be listening on " << port << endl;

	// initialize the use of Winsock DLL
	WSADATA wsaData;
	if ((error = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		cout << TimeStamp() << "Failed to initialize Winsock with error = " << error << endl;
		return error;
	}

	cout << TimeStamp() << "Initialized Winsock" << endl;

	SOCKET listenSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (listenSocket == INVALID_SOCKET) {
		error = WSAGetLastError();
		cout << TimeStamp() << "socket create failed with error = " << error << endl;
		WSACleanup();
		return error;
	}

	cout << TimeStamp() << "socket created" << endl;

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = INADDR_ANY;

	if (bind(listenSocket, (sockaddr*)&local, sizeof(local)) == SOCKET_ERROR) {
		error = WSAGetLastError();
		cout << TimeStamp() << "socket bind failed with error = " << error << endl;
		closesocket(listenSocket);
		WSACleanup();
		return error;
	}

	cout << TimeStamp() << "socket binded" << endl;

	struct sockaddr_in from;
	int fromlen = sizeof(from);

	while (true) {

		cout << endl << "Waiting..." << endl;

		char buffer[512] = {0};
		ZeroMemory(buffer, sizeof(buffer));

		int bytes_to_receive = sizeof(buffer);
		char * lpbuf = buffer;

		while (bytes_to_receive) {
			int len = recvfrom(listenSocket, buffer, sizeof(buffer), 0, (sockaddr*)&from, &fromlen);
			if (len == 0 || len == SOCKET_ERROR) break;

			cout << TimeStamp() << "Recieved : Size = " << len << " Message = " << buffer << endl << endl;

			bytes_to_receive -= len;
			lpbuf += len;
		}
	}

	closesocket(listenSocket);
	WSACleanup();
	return 0;
}