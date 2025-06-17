#pragma once

#include <string>
#include <unordered_map>
#include <optional>

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

	// 문자열을 MessageType으로 변환
	MessageType ParseMessageType(const std::string& typeStr);

	// MessageType을 문자열로 변환
	std::string ToString(MessageType type);

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
}