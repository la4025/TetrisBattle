#pragma once
#include "Protocol.h"

namespace Tetris
{
	// �� ȸ�� ���¿� ���� ����� �����ϴ� ��� ��ǥ�� ��ȯ
	std::vector<std::pair<int, int>> GetBlockCells(const BlockInfo& block);
}

