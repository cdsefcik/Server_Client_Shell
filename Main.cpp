#include <iostream>
#include <WS2tcpip.h>
#include<string>

#pragma comment(lib,"ws2_32.lib")

using namespace std; //Might take out.


void main() {
    string ipAddress = "127.0.0.1"; //Ip address of the server.
    int port = 54000;               //Listening port # on the server.

    //Initialize winsock
    WSADATA data;
    WORD ver MAKEWORD(2, 2);
    int wsResult = WSAStartup(ver, &data);

    if (wsResult != 0) {
        cerr << "Can't start winsock, Err #" << wsResult << endl;
        return;
    }

    //Create socket
    SOCKET SOCK = socket(AF_INET,SOCK_STREAM,0); //Stands for Address Family, internet version. The second paramter is the type of socket being created.
    if(SOCK == INVALID_SOCKET){
        cerr << "Can't create socket, err #" << WSAGetLastError() << endl;
        return;
    }
    
    //Fill in a hint structure. This tells what port we want to connect to.
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);


    //Connect to server
    int connResult = connect(SOCK, (sockaddr*)&hint, sizeof(hint));
    if (connResult == SOCKET_ERROR) {
        cerr << "Can't connect to server, err #" << WSAGetLastError() << endl;
        closesocket(SOCK);
        WSACleanup();
        return;
    }
    //Do-while loop to send and receive data
    char buf[4096];
    string userInput;

    do
    {
        //Prompt the user for some text.
        cout << "> ";
        getline(cin, userInput);
        //Send the text
        int sendResult = send(SOCK, userInput.c_str(), userInput.size() + 1, 0);
        if (sendResult != SOCKET_ERROR) {
            //Wait for response
            ZeroMemory(buf, 4096);
            int bytesReceived = recv(SOCK, buf, 4096, 0);
            if (bytesReceived > 0) {
                //Echo response to console
                cout << "<SERVER> " << string(buf, 0, bytesReceived) << endl;
            }
            
        }

    } while (userInput.size() > 0);
    
    //Gracefully close down everything
    closesocket(SOCK);
    WSACleanup();
}