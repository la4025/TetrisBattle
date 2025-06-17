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

	// 문자열을 MessageType으로 변환
	MessageType ParseMessageType(const std::string& typeStr);
	// MessageType을 문자열로 변환
	std::string ToString(MessageType type);

	// 문자열을 InputAction으로 변환
	InputAction ParseInputAction(const std::string& actionStr);
	// InputAction을 문자열로 변환
	std::string ToString(InputAction action);

	// 문자열을 TetrominoShape으로 변환
	TetrominoShape ParseShape(const std::string& actionStr);
	// TetrominoShape을 문자열로 변환
	std::string ToString(TetrominoShape shape);

	// 공통 메시지 구조
	struct Message
	{
		MessageType type;
		std::string payload; // JSON 문자열로 저장

		// 직렬화 : 전체 메시지를 JSON 문자열로 변환
		std::string Serialize() const;

		// 역직렬화 : JSON 문자열을 파싱하여 Message 구조로 생성
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

		// 구조체 -> JSON 문자열
		std::string Serialize() const;

		// JSON 문자열 -> 구조체
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