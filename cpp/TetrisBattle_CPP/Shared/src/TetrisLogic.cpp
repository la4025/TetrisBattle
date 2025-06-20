#include "../include/TetrisLogic.h"

namespace Tetris
{
	std::vector<std::pair<int, int>> Tetris::GetBlockCells(const BlockInfo& block)
	{
		std::vector<std::pair<int, int>> cells;

		int r = block.rotation % 4;

		switch (block.shape)
		{
		case TetrominoShape::I:
		{
			if (r == 0 || r == 2)
			{
				cells = { {0, 0}, {1, 0}, {2, 0}, {3, 0} };
			}
			else
			{
				cells = { {0, 0}, {0, 1}, {0, 2}, {0, 3} };
			}
			break;
		}
		case TetrominoShape::O:
		{
			cells = { {0, 0}, {1, 0}, {0, 1}, {1, 1} };
			break;
		}
		case TetrominoShape::T:
		{
			if (r == 0)
			{
				cells = { {0, 0}, {1, 0}, {2, 0}, {1, 1} };
			}
			else if (r == 1)
			{
				cells = { {1, 0}, {0, 1}, {1, 1}, {1, 2} };
			}
			else if (r == 2)
			{
				cells = { {0, 1}, {1, 0}, {1, 1}, {2, 1} };
			}
			else if (r == 3)
			{
				cells = { {0, 0}, {0, 1}, {1, 1}, {0, 2} };
			}
			break;
		}
		case TetrominoShape::S:
		{
			if (r == 0 || r == 2)
			{
				cells = { {1, 0}, {2, 0}, {0, 1}, {1, 1} };
			}
			else
			{
				cells = { {0, 0}, {0, 1}, {1, 1}, {2, 1} };
			}
			break;
		}
		case TetrominoShape::Z:
		{
			if (r == 0 || r == 2)
			{
				cells = { {0, 0}, {1, 0}, {1, 1}, {2, 1} };
			}
			else
			{
				cells = { {2, 0}, {0, 1}, {1, 1}, {0, 2} };
			}
			break;
		}
		case TetrominoShape::J:
		{
			if (r == 0)
			{
				cells = { {1, 0}, {1, 1}, {1, 2}, {0, 2} };
			}
			else if (r == 1)
			{
				cells = { {0, 0}, {0, 1}, {1, 1}, {2, 1} };
			}
			else if (r == 2)
			{
				cells = { {0, 0}, {1, 0}, {0, 1}, {0, 2} };
			}
			else if (r == 3)
			{
				cells = { {0, 0}, {1, 0}, {2, 0}, {2, 1} };
			}
			break;
		}
		case TetrominoShape::L:
		{
			if (r == 0)
			{
				cells = { {0, 0}, {0, 1}, {0, 2}, {1, 2} };
			}
			else if (r == 1)
			{
				cells = { {0, 0}, {1, 0}, {2, 0}, {0, 1} };
			}
			else if (r == 2)
			{
				cells = { {0, 0}, {1, 0}, {1, 1}, {1, 2} };
			}
			else if (r == 3)
			{
				cells = { {2, 0}, {0, 1}, {1, 1}, {2, 1} };
			}
			break;
		}
		default:
		{
			break;
		}
		}

		for (auto& cell : cells)
		{
			cell.first += block.x;
			cell.second += block.y;
		}

		return cells;
	}
}
