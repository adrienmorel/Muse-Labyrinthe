#include "Muse.h"
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <locale>
#include <sstream>
#include <conio.h>
using namespace std;
#pragma comment(lib,"ws2_32.lib")

#define DEFAULT_BUFLEN 10000

int recvbuflen = DEFAULT_BUFLEN;
char recvbuf[DEFAULT_BUFLEN];
int iResult;
char buffer[DEFAULT_BUFLEN];

Muse::Muse(){}

DWORD WINAPI Muse::get(LPVOID lpProgress) {

    unsigned int& progress = *((unsigned int*)lpProgress);
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0){
        cout << "WSAStartup failed.\n";
        system("pause");
        return 1;
    }

    string path = "/muse/result";

    string get_http;
    get_http = "GET " + path + " HTTP/1.1\r\nHost: " + this->domain + "\r\nConnection: close\r\n\r\n";

    SOCKADDR_IN SockAddr;

    struct hostent *host;
    host = gethostbyname(this->domain.c_str());
    SockAddr.sin_port=htons(80);
    SockAddr.sin_family=AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);


    while(true){
        SOCKET Socket;
        Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr)) != 0){
            cout << "Could not connect";
            //system("pause");
            WSACleanup();
            return 1;
        }

        iResult = send(Socket,get_http.c_str(), strlen(get_http.c_str()),0 );
        if (iResult == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
            closesocket(Socket);
            WSACleanup();
            return 1;
        }

        int nDataLength;
        while ((nDataLength = recv(Socket,buffer,10000,0)) > 0){
            string statue_str;
            statue_str += buffer[9];
            statue_str += buffer[10];
            statue_str += buffer[11];
            int statue = atoi(statue_str.c_str());
            switch (statue){
                case 404:
                    closesocket(Socket);
                    WSACleanup();
                    cout << "ERROR : ";
                    cout << statue;
                    return 1;
                case 503:
                    closesocket(Socket);
                    WSACleanup();
                    cout << "ERROR : ";
                    cout << statue;
                    return 1;
                default:
                    break;
            }

            int start = 0;
            while(buffer[start] != '{' && (buffer[start] >= 32 || buffer[start] == '\n' || buffer[start] == '\r')) start++;
            int end = start + 1;
            while(buffer[end] != '}' && (buffer[end] >= 32 || buffer[end] == '\n' || buffer[end] == '\r')) end++;

            string data_str;
            string success_str;
            for(int s = start; s <= end; s++){
                data_str += buffer[s];
            }
            size_t first = data_str.find('"', data_str.find('"', data_str.find('"') + 1) + 1);
            size_t last = data_str.find_last_of('"');
            string progress_str = data_str.substr(first+1,last-first-1);
            progress = (unsigned int)atoi(progress_str.c_str());
            cout << progress;
            Sleep(1000);
        }
    }
}


