#include <iostream>
#include <string>
#include <thread>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#include "../../Shared/include/Protocol.h"
#include "../../Shared/include/TetrisLogic.h"

using namespace Tetris;

// ��� �浹 Ȯ�� (���� �� ����)
bool IsCollision(const std::vector<std::vector<int>>& board, const BlockInfo& block)
{
	auto cells = GetBlockCells(block);
	int height = static_cast<int>(board.size());
	int width = static_cast<int>(board[0].size());

	for (const auto& [x, y] : cells)
	{
		if (x < 0 || x >= width || y >= height)
		{
			return true; // �� �浹 �Ǵ� �ٴ�
		}

		if (y >= 0 && board[y][x] != 0)
		{
			return true; // ���� ���� �浹
		}
	}

	return false;
}

// ��� ���忡 ����
void FixBlockToBoard(std::vector<std::vector<int>>& board, const BlockInfo& block)
{
	auto cells = GetBlockCells(block);
	int height = static_cast<int>(board.size());
	int width = static_cast<int>(board[0].size());
	
	for (const auto& [x, y] : cells)
	{
		if (y >= 0 && y < height &&	x >= 0 && x < width)
		{
			board[y][x] = 1; // ���� ���縦 1�� ǥ��
		}
	}
}

// ����� ������ ��ġ�� �̹� �����ִ��� Ȯ��
bool IsGameOver(const std::vector<std::vector<int>>& board, const BlockInfo& block)
{
	return IsCollision(board, block);
}

// �ؿ��� ���� ä���� �� ����
void ClearFullLines(std::vector<std::vector<int>>& board)
{
	int height = static_cast<int>(board.size());
	int width = static_cast<int>(board[0].size());

	std::vector<std::vector<int>> newBoard;

	for (int y = height - 1; y >= 0; --y)
	{
		int filled = 0;

		for (int x = 0; x < width; ++x)
		{
			if (board[y][x] != 0)
			{
				filled++;
			}
		}

		if (filled < width)
		{
			newBoard.push_back(board[y]);
		}
	}

	// ����� �ٸ�ŭ �� ���� �� �� ����
	int newHeight = static_cast<int>(board.size());
	while (newHeight < height)
	{
		newBoard.emplace_back(width, 0);
	}

	// ���� ��ü 
	// newBoard�� ������ �����Ƿ� �ٽ� ����� ��ü
	std::reverse(newBoard.begin(), newBoard.end());
	board = newBoard;
}

int main()
{
	//WinSock �ʱ�ȭ
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

	// ������ �ʱ�ȭ
	std::vector<std::vector<int>> board(20, std::vector<int>(10, 0));
	BlockInfo currentBlock{ TetrominoShape::T, 3, 0, 0 };
	TetrominoShape nextBlock = TetrominoShape::L;

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
						std::cerr << "Message::Deserialize ����\n";
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

					// �Է� ó��
					BlockInfo temp = currentBlock;

					switch (input->action)
					{
					case InputAction::MoveLeft:
					{
						temp.x--;
						break;
					}
					case InputAction::MoveRight:
					{
						temp.x++;
						break;
					}
					case InputAction::Rotate:
					{
						temp.rotation = (temp.rotation + 1) % 4;

						if (!IsCollision(board, temp))
						{
							currentBlock.rotation = temp.rotation;
						}

						break;
					}
					case InputAction::SoftDrop:
					{
						temp.y++;
						break;
					}
					case InputAction::HardDrop:
					{
						BlockInfo dropped = temp;

						while (true)
						{
							BlockInfo next = dropped;
							next.y++;

							if (IsCollision(board, next))
							{
								break;
							}
							dropped = next;
						}

						FixBlockToBoard(board, dropped);
						ClearFullLines(board);

						temp = BlockInfo{ nextBlock, 3, 0, 0 };
						nextBlock = static_cast<TetrominoShape>(rand() % 7);

						if (IsGameOver(board, currentBlock))
						{
							std::cout << "[����] ���� ����!\n";
							break;
						}

						break;
					}
					default:
					{
						break;
					}
					}

					if (!IsCollision(board, temp))
					{
						currentBlock = temp;
					}

				}
				catch (const std::exception& ex)
				{
					std::cerr << "���ܹ߻� : " << ex.what() << std::endl;
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

		// ��� ���� ó�� (0.5 �ʸ���)
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastDropTime);

		if (elapsed.count() >= 500)
		{
			BlockInfo temp = currentBlock;
			temp.y++;

			if (IsCollision(board, temp))
			{
				FixBlockToBoard(board, currentBlock);
				ClearFullLines(board);

				currentBlock = BlockInfo{ nextBlock,3, 0, 0 };
				nextBlock = static_cast<TetrominoShape>(rand() % 7);

				if (IsGameOver(board, currentBlock))
				{
					std::cout << "[����] ���� ����!\n";
					break;
				}
			}
			else
			{
				currentBlock = temp;
			}

			lastDropTime = now;
		}

		// GameState ����
		GameStatePayload state;
		state.board = board;
		state.currentBlock = currentBlock;
		state.nextBlock = nextBlock;

		Message response;
		response.type = MessageType::GameState;
		response.payload = state.Serialize();

		std::string toSend = response.Serialize();
		send(clientSocket, toSend.c_str(), static_cast<int>(toSend.size()), 0);
		std::cout << "[����] GameInput ����, ���� ����" << std::endl; // ����� �α�

		std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 20fps
	}

	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();

	return 0;
}