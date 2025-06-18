#include <iostream>
#include <string>
#include <thread>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#include "../../Shared/include/Protocol.h"

using namespace Tetris;

int main()
{
	//WinSock 초기화
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

	SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
	std::cout << "[Server] Client connected!\n";

	u_long mode = 1;
	ioctlsocket(clientSocket, FIONBIO, &mode);

	// 게임판 초기화
	std::vector<std::vector<int>> board(20, std::vector<int>(10, 0));
	int currentX = 4;
	int currentY = 0;

	std::string recvBuffer;
	auto lastDropTime = std::chrono::steady_clock::now();

	while (true)
	{
		char buffer[4096];
		int received = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (received > 0)
		{
			recvBuffer.append(buffer, received);
			size_t pos;
			while ((pos = recvBuffer.find('\n')) != std::string::npos)
			{
				std::string data = recvBuffer.substr(0, pos);
				recvBuffer.erase(0, pos + 1);

				try
				{
					auto msg = Message::Deserialize(data);

					if (!msg) {
						std::cerr << "Message::Deserialize 실패\n";
						continue;
					}

					std::cout << "type: " << static_cast<int>(msg->type) << std::endl;

					if (msg->type != MessageType::GameInput) {
						std::cerr << "msg->type != GameInput (" << static_cast<int>(msg->type) << ")\n";
						continue;
					}

					auto input = GameInputPayload::Deserialize(msg->payload);

					if (!input)
					{
						continue;
					}

					// 입력 처리
					switch (input->action)
					{
					case InputAction::MoveLeft:
					{
						if (currentX > 0)
						{
							currentX--;
							break;
						}
					}
					case InputAction::MoveRight:
					{
						if (currentX < 9)
						{
							currentX++;
							break;
						}
					}
					case InputAction::SoftDrop:
					{
						if (currentY < 19)
						{
							currentY++;
							break;
						}
					}
					case InputAction::HardDrop:
					{
						currentY = 19;
						break;
					}
					default:
					{
						break;
					}
					}
				}
				catch (const std::exception& ex)
				{
					std::cerr << "예외발생 : " << ex.what() << std::endl;
					continue;
				}
			}
		}
		else if (received == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				std::cerr << "recv error\n";
				break;
			}
		}

		// 블록 낙하 처리 (0.5 초마다)
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastDropTime);

		if (elapsed.count() >= 500)
		{
			if (currentY < 19)
			{
				currentY++;
			}
			lastDropTime = now;
		}

		// GameState 생성
		GameStatePayload state;
		state.board = board;
		state.currentBlock = BlockInfo{ TetrominoShape::T, currentX, currentY, 0 };
		state.nextBlock = TetrominoShape::L;

		Message response;
		response.type = MessageType::GameState;
		response.payload = state.Serialize();

		std::string toSend = response.Serialize();
		send(clientSocket, toSend.c_str(), static_cast<int>(toSend.size()), 0);
		std::cout << "[서버] GameInput 수신, 응답 전송" << std::endl; // 디버깅 로그

		std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 20fps
	}

	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();

	return 0;
}