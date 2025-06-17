#include "../include/Protocol.h"
#include <sstream>
#include <algorithm>

namespace Tetris
{
	// helper : 문자열 트리밍
	static std::string TrimQuotes(const std::string& str)
	{
		if (str.front() == '"' && str.back() == '"')
		{
			return str.substr(1, str.size() - 2);
		}

		return str;
	}

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

	std::string Message::Serialize() const
	{
		std::ostringstream oss;
		oss << "{";
		oss << "\"type\":\"" << ToString(type) << "\",";
		oss << "\"payload\":" << payload;
		oss << "}";

		return oss.str();
	}

	std::optional<Message> Message::Deserialize(const std::string& jsonStr)
	{
		Message msg;

		auto typePos = jsonStr.find("\"type\"");

		if (typePos == std::string::npos)
		{
			return std::nullopt;
		}

		auto typeStart = jsonStr.find(":", typePos);
		auto typeEnd = jsonStr.find(",", typeStart);

		if (typeStart == std::string::npos || typeEnd == std::string::npos)
		{
			return std::nullopt;
		}

		std::string typeStr = jsonStr.substr(typeStart + 1, typeEnd - typeStart - 1);
		msg.type = ParseMessageType(TrimQuotes(typeStr));

		auto payloadPos = jsonStr.find("\"payload\"");

		if (payloadPos == std::string::npos)
		{
			return std::nullopt;
		}

		auto payloadStart = jsonStr.find(":", payloadPos);

		if (payloadStart == std::string::npos)
		{
			return std::nullopt;
		}

		std::string payload = jsonStr.substr(payloadStart + 1);

		if (!payload.empty() && payload.back() == '}')
		{
			// 끝의 } 제거
			size_t lastBrace = payload.find_last_of('}');
			payload = payload.substr(0, lastBrace + 1);
		}

		msg.payload = payload;

		return msg;
	}

}