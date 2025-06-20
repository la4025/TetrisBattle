#pragma once
#include "Protocol.h"

namespace Tetris
{
	// 각 회전 상태에 따라 블록이 차지하는 상대 좌표를 반환
	std::vector<std::pair<int, int>> GetBlockCells(const BlockInfo& block);
}

