#pragma once

#include <string>
#include <unordered_map>
#include <optional>

#include "../include/nlohmann/json.hpp"
using json = nlohmann::json;

namespace Tetris
{
	enum class MessageType
	{
		Unknown,
		Login,
		LoginResult,
		MatchFound,
		GameInput,
		GameState,
		GameOver
	};

	enum class InputAction
	{
		Unknown,
		MoveLeft,
		MoveRight,
		Rotate,
		SoftDrop,
		HardDrop,
		Hold
	};

	enum class TetrominoShape
	{
		None,
		I,
		O,
		T,
		S,
		Z,
		J,
		L
	};

	// ���ڿ��� MessageType���� ��ȯ
	MessageType ParseMessageType(const std::string& typeStr);
	// MessageType�� ���ڿ��� ��ȯ
	std::string ToString(MessageType type);

	// ���ڿ��� InputAction���� ��ȯ
	InputAction ParseInputAction(const std::string& actionStr);
	// InputAction�� ���ڿ��� ��ȯ
	std::string ToString(InputAction action);

	// ���ڿ��� TetrominoShape���� ��ȯ
	TetrominoShape ParseShape(const std::string& actionStr);
	// TetrominoShape�� ���ڿ��� ��ȯ
	std::string ToString(TetrominoShape shape);

	// ���� �޽��� ����
	struct Message
	{
		MessageType type;
		std::string payload; // JSON ���ڿ��� ����

		// ����ȭ : ��ü �޽����� JSON ���ڿ��� ��ȯ
		std::string Serialize() const;

		// ������ȭ : JSON ���ڿ��� �Ľ��Ͽ� Message ������ ����
		static std::optional<Message> Deserialize(const std::string& jsonStr);
	};

	struct GameInputPayload
	{
		InputAction action;
		uint64_t timestamp;

		std::string Serialize() const;
		static std::optional<GameInputPayload> Deserialize(const std::string& jsonStr);
	};

	struct LoginPayload
	{
		std::string username;
		std::string password;

		// ����ü -> JSON ���ڿ�
		std::string Serialize() const;

		// JSON ���ڿ� -> ����ü
		static std::optional<LoginPayload> Deserialize(const std::string& jsonStr);
	};

	struct LoginResultPayload
	{
		bool success;
		std::string message;
		int userId;

		std::string Serialize() const;
		static std::optional<LoginResultPayload> Deserialize(const std::string& jsonStr);
	};

	struct BlockInfo
	{
		TetrominoShape shape;
		int x;
		int y;
		int rotation;

		json ToJson() const;
		static BlockInfo FromJson(const json& jsonData);
	};

	struct GameStatePayload
	{
		std::vector<std::vector<int>> board;
		BlockInfo currentBlock;
		TetrominoShape nextBlock;

		std::string Serialize() const;
		static std::optional<GameStatePayload> Deserialize(const std::string& jsonStr);
	};
}