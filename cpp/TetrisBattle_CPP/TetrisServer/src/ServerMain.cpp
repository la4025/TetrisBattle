#include <iostream>
#include <string>
#include <thread>
#include<WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#include "../../Shared/include/Protocol.h"

void HandleClient(SOCKET clientSocket)
{
	char buffer[1024];
	int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

	if (bytesReceived <= 0)
	{
		return;
	}

	std::string received(buffer, bytesReceived);
	std::cout << "[Server] Received : " << received << std::endl;

	auto parsed = Tetris::Message::Deserialize(received);

	if (parsed.has_value())
	{
		std::string response = parsed->Serialize();
		send(clientSocket, response.c_str(), static_cast<int>(response.size()), 0);
		std::cout << "[Server] Echoed back.\n";
	}

	closesocket(clientSocket);
}

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5000);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	listen(serverSocket, SOMAXCONN);

	std::cout << "[Server] Listening on port 5000....\n";

	while (true)
	{
		SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
		std::thread(HandleClient, clientSocket).detach();
	}

	WSACleanup();
}