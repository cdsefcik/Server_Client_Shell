#include <iostream>
#include<WS2tcpip.h> //Includes the winsock file as well

#pragma comment(lib, "ws2_32.lib")

using namespace std;

void main() {

	//Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2); //Request the version.
	int wsOK = WSAStartup(ver, &wsData);
	if (wsOK != 0) {
		cerr << "Cant initialize winsock! Quitting" << endl;
		return;
	}

	//Create a socket: A scoket is an endpoint, matched with a port and an IP address.
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0); //Using a TCP socket (SOCK_STREAM
		if (listening == INVALID_SOCKET) {
			cerr << "Cant create a socket! Quitting" << endl;
			return;
	}

	//Bind the socket to an ip address and port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000); //This changes to big endian, which is for networking. Means: host to network short.
	hint.sin_addr.S_un.S_addr = INADDR_ANY; //Could also use unet_pton....
	bind(listening, (sockaddr*)&hint, sizeof(hint));


	//Tell the winsock the socket is for listening
	listen(listening, SOMAXCONN); //Number of maximum connections we can have in backlog

	//Wait for a connection
	sockaddr_in client;
	int clientsize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientsize); //listening for connection
	//if(clientSocket == INVALID_SOCKET) //Do something NOTE:INISTIATE LATER
	char host[NI_MAXHOST];    //Clients remote name
	char service[NI_MAXHOST]; //Service(i.e. port) the client is connect on.

	ZeroMemory(host, NI_MAXHOST); //
	ZeroMemory(service, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		cout << host << "Connected on port " << service << endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " Connected on port " <<
			ntohs(client.sin_port) << endl;
	}
	
	//Close the listening socket
	closesocket(listening);

	//While loop: accept and echo message back to client
	char buf[4096]; //Note, for the future, we should read until we receive no more bytes.

	while (true) {
		ZeroMemory(buf, 4096);

		//Wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 4096, 0);//
		if (bytesReceived == SOCKET_ERROR) {
			cerr << "Error in recv(). Quitting" << endl;
			break;
		}

		if (bytesReceived == 0) {
			cout << "Client disconnected " << endl;
			break;
		}

		cout << string(buf, 0, bytesReceived)<<endl;
		//Echo message back to client
		send(clientSocket, buf, bytesReceived + 1, 0);

		
	}

	//Close the socket
	closesocket(clientSocket);

	//shut down winsock
	WSACleanup();

}