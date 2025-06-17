#include <iostream>
#include <string>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include "../../Shared/include/Protocol.h"

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5000);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

	Tetris::Message loginMsg;
	loginMsg.type = Tetris::MessageType::Login;
	loginMsg.payload = "{\"username\":\"player1\",\"password\":\"pw123\"}";

	std::string data = loginMsg.Serialize();
	send(sock, data.c_str(), static_cast<int>(data.size()), 0);

	char buffer[1024];
	int received = recv(sock, buffer, sizeof(buffer), 0);

	if (received > 0)
	{
		std::string response(buffer, received);
		std::cout << "[Client] Received from server : " << response << "\n";

		auto parsed = Tetris::Message::Deserialize(response);

		if (parsed.has_value())
		{
			std::cout << "[Client] Parsed type : " << Tetris::ToString(parsed->type) << "\n";
			std::cout << "[Client] Payload : " << parsed->payload << "\n";
		}
	}

	closesocket(sock);
	WSACleanup();
}