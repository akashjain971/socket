// server.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <string>
#include <time.h>
#include <winsock2.h>
#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable : 4996)

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
    cout << TimeStamp() << "Server will be listening on port " << port << endl;

    // initialize the use of Winsock DLL
    WSADATA wsaData;
    if ((error = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
        cout << TimeStamp() << "Failed to initialize Winsock with error = " << error << endl;
        return error;
    }

    cout << TimeStamp() << "Initialized Winsock" << endl;

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        error = WSAGetLastError();
        cout << TimeStamp() << "socket create failed with error = " << error << endl;
        WSACleanup();
        return error;
    }

    cout << TimeStamp() << "socket created" << endl;

    struct sockaddr_in server;
    ZeroMemory(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenSocket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        error = WSAGetLastError();
        cout << TimeStamp() << "socket bind failed with error = " <<  error << endl;
        closesocket(listenSocket);
        WSACleanup();
        return error;
    }

    cout << TimeStamp() << "socket binded" << endl;

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        error = WSAGetLastError();
        cout << TimeStamp() << "socket listen failed with error = " << error << endl;
        closesocket(listenSocket);
        WSACleanup();
        return error;
    }

    cout << TimeStamp() << "socket listening" << endl;

    while (true) {
        
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            error = WSAGetLastError();
            cout << TimeStamp() << "socket accept failed with error = " << error << endl;
            closesocket(listenSocket);
            WSACleanup();
            return error;
        }

        cout << TimeStamp() << "socket accepted" << endl;

        while (true) {

            cout << "Waiting for client to send..." << endl;

            int header_length = 4;
            char bufferlen[4] = {0};

            int ret = recv(clientSocket, bufferlen, header_length, 0);
            error = WSAGetLastError();
            if (error == WSAECONNRESET) break;
            if (ret == 0 || ret == SOCKET_ERROR) break;

            int bytes_to_receive = *(unsigned int*)bufferlen;
            char * buffer = (char*)calloc(bytes_to_receive, sizeof(bytes_to_receive));

            cout << TimeStamp() << "Received Header: Message size = " << bytes_to_receive << endl;

            while (bytes_to_receive) {

                int received = recv(clientSocket, buffer, bytes_to_receive, 0);
                if (received == 0 || received == SOCKET_ERROR) break;

                cout << TimeStamp() << "Received Message: Total size = " << bytes_to_receive + header_length << " Message = " << buffer << endl;

                bytes_to_receive -= received;
                buffer += received;
            }

            char ack[15] = "Got it!";
            send(clientSocket, ack, sizeof(ack), 0);
            cout << TimeStamp() << "Acknowledgement sent to client" << endl << endl;

        }

        closesocket(clientSocket);
    }
    
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
