#pragma once

#include <memory>
#include "Tetromino.h"
#include "Board.h"

std::unique_ptr<Tetromino> GenerateRandomTetromino(Board& board);
