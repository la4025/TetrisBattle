#include "../include/Protocol.h"
#include <sstream>
#include <algorithm>

namespace Tetris
{
	MessageType ParseMessageType(const std::string& typeStr)
	{
		if (typeStr == "Login")
		{
			return MessageType::Login;
		}
		if (typeStr == "LoginResult")
		{
			return MessageType::LoginResult;
		}
		if (typeStr == "MatchFound")
		{
			return MessageType::MatchFound;
		}
		if (typeStr == "GameInput")
		{
			return MessageType::GameInput;
		}
		if (typeStr == "GameState")
		{
			return MessageType::GameState;
		}
		if (typeStr == "GameOver")
		{
			return MessageType::GameOver;
		}

		return MessageType::Unknown;
	}

	std::string ToString(MessageType type)
	{
		switch (type)
		{
		case MessageType::Login:
		{
			return "Login";
		}
		case MessageType::LoginResult:
		{
			return "LoginResult";
		}
		case MessageType::MatchFound:
		{
			return "MatchFound";
		}
		case MessageType::GameInput:
		{
			return "GameInput";
		}
		case MessageType::GameState:
		{
			return "GameState";
		}
		case MessageType::GameOver:
		{
			return "GameOver";
		}
		default:
		{
			return "Unknown";
		}
		}
	}

	InputAction ParseInputAction(const std::string& actionStr)
	{
		if (actionStr == "MoveLeft")
		{
			return InputAction::MoveLeft;
		}
		if (actionStr == "MoveRight")
		{
			return InputAction::MoveRight;
		}
		if (actionStr == "Rotate")
		{
			return InputAction::Rotate;
		}
		if (actionStr == "SoftDrop")
		{
			return InputAction::SoftDrop;
		}
		if (actionStr == "HardDrop")
		{
			return InputAction::HardDrop;
		}
		if (actionStr == "Hold")
		{
			return InputAction::Hold;
		}

		return InputAction::Unknown;
	}

	std::string ToString(InputAction action)
	{
		switch (action)
		{
		case InputAction::MoveLeft:
		{
			return "MoveLeft";
		}
		case InputAction::MoveRight:
		{
			return "MoveRight";
		}
		case InputAction::Rotate:
		{
			return "Rotate";
		}
		case InputAction::SoftDrop:
		{
			return "SoftDrop";
		}
		case InputAction::HardDrop:
		{
			return "HardDrop";
		}
		case InputAction::Hold:
		{
			return "Hold";
		}
		default:
		{
			return "Unknown";
		}
		}
	}

	TetrominoShape ParseShape(const std::string& actionStr)
	{
		if (actionStr == "I")
		{
			return TetrominoShape::I;
		}
		if (actionStr == "O")
		{
			return TetrominoShape::O;
		}
		if (actionStr == "T")
		{
			return TetrominoShape::T;
		}
		if (actionStr == "S")
		{
			return TetrominoShape::S;
		}
		if (actionStr == "Z")
		{
			return TetrominoShape::Z;
		}
		if (actionStr == "J")
		{
			return TetrominoShape::J;
		}
		if (actionStr == "L")
		{
			return TetrominoShape::L;
		}

		return TetrominoShape::None;
	}

	std::string ToString(TetrominoShape shape)
	{
		switch (shape)
		{
		case TetrominoShape::I:
		{
			return "I";
		}
		case TetrominoShape::O:
		{
			return "O";
		}
		case TetrominoShape::T:
		{
			return "T";
		}
		case TetrominoShape::S:
		{
			return "S";
		}
		case TetrominoShape::Z:
		{
			return "Z";
		}
		case TetrominoShape::J:
		{
			return "J";
		}
		case TetrominoShape::L:
		{
			return "L";
		}
		default:
		{
			return "None";
		}
		}
	}

	std::string Message::Serialize() const
	{
		json jsonData;
		jsonData["type"] = ToString(type);
		// payload는 JSON 문자열 이므로 파싱
		jsonData["payload"] = json::parse(payload);

		return jsonData.dump();
	}

	std::optional<Message> Message::Deserialize(const std::string& jsonStr)
	{
		try
		{
			json jsonData = json::parse(jsonStr);

			Message msg;
			msg.type = ParseMessageType(jsonData.at("type").get<std::string>());
			// payload는 다시 문자열로 저장
			msg.payload = jsonData.at("payload").dump();
		}
		catch (...)
		{
			return std::nullopt;
		}
	}

	std::string GameInputPayload::Serialize() const
	{
		json jsonData;
		jsonData["action"] = ToString(action);
		jsonData["timestamp"] = timestamp;

		return jsonData.dump();
	}

	std::optional<GameInputPayload> GameInputPayload::Deserialize(const std::string& jsonStr)
	{
		try
		{
			json jsonData = json::parse(jsonStr);
			
			GameInputPayload payload;
			payload.action = ParseInputAction(jsonData.at("action").get<std::string>());
			payload.timestamp = jsonData.at("timestamp").get<uint64_t>();

			return payload;
		}
		catch (...)
		{
			return std::nullopt;
		}
	}

	std::string LoginPayload::Serialize() const
	{
		json jsonData;
		jsonData["username"] = username;
		jsonData["password"] = password;
		return jsonData.dump();
	}

	std::optional<LoginPayload> LoginPayload::Deserialize(const std::string& jsonStr)
	{
		try
		{
			json jsonData = json::parse(jsonStr);
			LoginPayload payload;
			payload.username = jsonData.at("username").get<std::string>();
			payload.password = jsonData.at("password").get<std::string>();

			return payload;
		}
		// 모든 예외를 처리
		catch (...)
		{
			return std::nullopt;
		}
	}

	std::string LoginResultPayload::Serialize() const
	{
		json jsonData;
		jsonData["success"] = success;
		jsonData["message"] = message;
		jsonData["userId"] = userId;

		return jsonData.dump();
	}

	std::optional<LoginResultPayload> LoginResultPayload::Deserialize(const std::string& jsonStr)
	{
		try
		{
			json jsonData;
			LoginResultPayload payload;
			payload.success = jsonData.at("success").get<bool>();
			payload.message = jsonData.at("message").get<std::string>();
			payload.userId = jsonData.at("userId").get<int>();

			return payload;
		}
		catch (...)
		{
			return std::nullopt;
		}
	}

	json BlockInfo::ToJson() const
	{
		json jsonData;
		jsonData["shape"] = ToString(shape);
		jsonData["x"] = x;
		jsonData["y"] = y;
		jsonData["rotation"] = rotation;

		return jsonData;
	}

	BlockInfo BlockInfo::FromJson(const json& jsonData)
	{
		BlockInfo b;
		b.shape = ParseShape(jsonData.at("shape").get<std::string>());
		b.x = jsonData.at("x").get<int>();
		b.y = jsonData.at("y").get<int>();
		b.rotation = jsonData.at("rotation").get<int>();

		return b;
	}

	std::string GameStatePayload::Serialize() const
	{
		json jsonData;
		jsonData["board"] = board;
		jsonData["currentBlock"] = currentBlock.ToJson();
		jsonData["nextBlock"] = ToString(nextBlock);

		return jsonData;
	}

	std::optional<GameStatePayload> GameStatePayload::Deserialize(const std::string& jsonStr)
	{
		try
		{
			json jsonData;
			GameStatePayload payload;
			payload.board = jsonData.at("board").get<std::vector<std::vector<int>>>();
			payload.currentBlock = BlockInfo::FromJson(jsonData.at("currentBlock"));
			payload.nextBlock = ParseShape(jsonData.at("nextBlock").get<std::string>());

			return payload;
		}
		catch (...)
		{
			return std::nullopt;
		}
	}

}