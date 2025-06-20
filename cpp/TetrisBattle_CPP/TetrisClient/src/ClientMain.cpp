#include <iostream>
#include <string>
#include <thread>
#include <conio.h>
#include <chrono>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <vector>
#include "../../Shared/include/Protocol.h"
#include "../../Shared/include/TetrisLogic.h"

using namespace Tetris;

InputAction GetInputActionFromKey(char key)
{
	switch (key)
	{
	case 'a':
	{
		return InputAction::MoveLeft;
	}
	case 'd':
	{
		return InputAction::MoveRight;
	}
	case 'w':
	{
		return InputAction::Rotate;
	}
	case 's':
	{
		return InputAction::SoftDrop;
	}
	case ' ':
	{
		return InputAction::HardDrop;
	}
	default:
	{
		return InputAction::Unknown;
	}
	}
}

uint64_t GetCurrentTimestamp()
{
	using namespace std::chrono;

	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void RenderGameState(const GameStatePayload& state);

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5000);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) != 0)
	{
		std::cerr << "서버 연결 실패\n";

		return 1;
	}

	// 응답 대기 (non-blocking 방식 간단 구현)
	u_long mode = 1;
	// non-blocking 모드
	ioctlsocket(sock, FIONBIO, &mode);

	std::cout << "서버에 연결됨. 조작키 : A/D/W/S/SPACE\n";

	while (true)
	{
		// 입력감지
		if (_kbhit())
		{
			char key = _getch();
			InputAction action = GetInputActionFromKey(key);

			if (action == InputAction::Unknown)
			{
				continue;
			}

			// 입력 메시지 생성
			GameInputPayload input;
			input.action = action;
			input.timestamp = GetCurrentTimestamp();

			Message msg;
			msg.type = MessageType::GameInput;
			msg.payload = input.Serialize();
			std::string toSend = msg.Serialize() + "\n"; // 메시지 끝에 구분자 추가

			send(sock, toSend.c_str(), static_cast<int>(toSend.size()), 0);
		}

		
		char buffer[4096] = {};
		int received = recv(sock, buffer, sizeof(buffer), 0);

		if (received > 0)
		{
			std::string response(buffer, received);
			//std::cout << "[클라이언트 수신] " << response << std::endl; // 디버깅 로그

			auto parsed = Message::Deserialize(response);

			if (parsed) // 디버깅 로그
			{
				//std::cout << "메시지 타입: " << static_cast<int>(parsed->type) << std::endl; // 디버깅 로그


				if (parsed && parsed->type == MessageType::GameState)
				{
					auto state = GameStatePayload::Deserialize(parsed->payload);

					if (state) 
					{
						//std::cout << "GameStatePayload 파싱 성공\n"; // 디버깅 로그
						RenderGameState(*state);
					}
					else 
					{
						//std::cerr << "GameStatePayload 파싱 실패\n"; // 디버깅 로그
					}
				}
			}
		}

		// 20fps
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	closesocket(sock);
	WSACleanup();

	return 0;
}


void RenderGameState(const GameStatePayload& state) 
{
	const int height = state.board.size();
	const int width = state.board[0].size();

	// 회전 적용된 블록 셀 좌표 계산
	auto blockCells = GetBlockCells(state.currentBlock);

	// 클리어 화면
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif

	// 게임판 출력
	for (int y = 0; y < height; ++y) 
	{
		for (int x = 0; x < width; ++x) 
		{
			bool isBlock = std::any_of(blockCells.begin(), blockCells.end(),
				[&](const std::pair<int, int>& cell)
				{
					return cell.first == x && cell.second == y;
				});

			if (isBlock)
			{
				std::cout << "□";
			}
			else 
			{
				std::cout << (state.board[y][x] ? "■" : " ");
			}
		}
		std::cout << "\n";
	}

	// 다음 블록 정보 출력
	std::cout << "\nNext: " << ToString(state.nextBlock) << "\n";
}