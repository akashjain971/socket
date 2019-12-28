// client.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <string>
#include <time.h>
#include <winsock2.h>
#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable : 4996)

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
    if (argc > 3 && argv[3] && strcmp(argv[3], "reuse=false") == 0) reuseSocket = false;

    cout << TimeStamp() << "Client will be sending on " << ip << ":" << port << endl;

    // initialize the use of Winsock DLL
    WSADATA wsaData;
    if((error = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
        cout << TimeStamp() << "Failed to initialize Winsock with error = " << error << endl;
        return error;
    } 
    
    cout << TimeStamp() << "Initialized Winsock" << endl;

    do {

        SOCKET sendSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sendSocket == INVALID_SOCKET) {
            error = WSAGetLastError();
            cout << TimeStamp() << "socket create failed with error = " << error << endl;
            WSACleanup();
            return error;
        }

        cout << TimeStamp() << "socket created" << endl;

        struct sockaddr_in serverInfo;
        ZeroMemory(&serverInfo, sizeof(serverInfo));

        serverInfo.sin_family = AF_INET;
        serverInfo.sin_port = htons(port);
        serverInfo.sin_addr.s_addr = inet_addr(ip.c_str());

        if (connect(sendSocket, (sockaddr*)&serverInfo, sizeof(serverInfo)) == SOCKET_ERROR) {
            error = WSAGetLastError();
            cout << TimeStamp() << "socket connect failed with error = " << error << endl;
            closesocket(sendSocket);
            WSACleanup();
            return error;
        }

        cout << TimeStamp() << "socket connected" << endl;

        do {
            char buffer[1024] = { 0 };
            int header_length = 4;

            cout << "Please enter your message: ";
            cin.getline(buffer + header_length, sizeof(buffer) - header_length);

            int payload_length = strlen(buffer + header_length);
            *(unsigned int *)buffer = payload_length;

            int bytes_to_send = header_length + payload_length;
            char * lpbuf = buffer;

            while (bytes_to_send) {

                int len = send(sendSocket, lpbuf, bytes_to_send, 0);
                error = WSAGetLastError();
                if (len == 0 || len == SOCKET_ERROR) break;

                cout << TimeStamp() << "Sent : Total size = " << len << " Message = " << buffer + header_length << endl;

                bytes_to_send -= len;
                lpbuf += len;
            }

            if (error == WSAECONNRESET) break;

            char ack[15] = {0};
            recv(sendSocket, ack, sizeof(ack), 0);
            cout << TimeStamp() << "Server acknowledgement : " << ack << endl << endl;

            cout << (reuseSocket ? "Reusing the same socket..." : "Creating a new socket...") << endl;

        } while (reuseSocket);

        closesocket(sendSocket);

    } while (!reuseSocket);

    WSACleanup();
    return 0;
}

