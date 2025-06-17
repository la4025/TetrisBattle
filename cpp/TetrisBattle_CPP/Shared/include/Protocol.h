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

	// ���ڿ��� MessageType���� ��ȯ
	MessageType ParseMessageType(const std::string& typeStr);

	// MessageType�� ���ڿ��� ��ȯ
	std::string ToString(MessageType type);

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
}