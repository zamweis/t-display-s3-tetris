#ifndef TETRISAI_H
#define TETRISAI_H

#include "Game/BlockMap.h"
#include "Model/Shapes/Shape.h"
#include <vector>

class TetrisAI {
public:
    TetrisAI();
    ~TetrisAI();

    // Struct to store move evaluation results
    struct Move {
        int x;
        int rotation;
        int score;
    };

    // Evaluates and selects the best move for a given shape
    Move findBestMove(const BlockMap& blockMap, const Shape& shape);

private:
    // Utility methods for evaluating moves
    int evaluatePlacement(const BlockMap& blockMap, const Shape& shape, int x, int rotation);
    int calculateScore(const BlockMap& blockMap);

    // Weights for scoring heuristic (you can tune these values)
    int heightWeight;
    int holeWeight;
    int bumpinessWeight;

    // Helper function to simulate shape placement
    BlockMap simulatePlacement(const BlockMap& blockMap, const Shape& shape, int x, int rotation);
};

#endif // TETRISAI_H
