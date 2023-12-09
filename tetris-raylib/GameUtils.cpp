#include <random>
#include <memory>
#include "Board.h"
#include "Tetromino.h"

std::unique_ptr<Tetromino> GenerateRandomTetromino(Board& board) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 6);

    int tetrominoType = dis(gen);

    switch (tetrominoType) {
    case 0: return std::make_unique<StraightTetromino>(board);
    case 1: return std::make_unique<SquareTetromino>(board);
    case 2: return std::make_unique<TeeTetromino>(board);
    case 3: return std::make_unique<JayTetromino>(board);
    case 4: return std::make_unique<EllTetromino>(board);
    case 5: return std::make_unique<SkewSTetromino>(board);
    case 6: return std::make_unique<SkewZTetromino>(board);
    default: throw std::runtime_error("Invalid Tetromino type");
    }
}
