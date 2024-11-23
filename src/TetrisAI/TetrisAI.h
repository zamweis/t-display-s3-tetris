#ifndef TETRISAI_H
#define TETRISAI_H

#include "Game/BlockMap.h"
#include "Model/Shapes/Shape.h"

class TetrisAI {
public:
    // Structure to represent a move decision (x-position, rotation, and its heuristic score)
    struct Move {
        int x;
        int rotation;
        int score;
    };

    // Constructor and Destructor
    TetrisAI();
    ~TetrisAI();

    // Method to find the best move given the current state of the game
    Move findBestMove(const BlockMap& blockMap, const Shape& shape);


private:
    // Heuristic weights (can be adjusted for different AI behavior)
    int heightWeight;
    int holeWeight;
    int bumpinessWeight;
    int lineClearWeight;

    // Helper methods for evaluating and scoring potential moves
    int evaluatePlacement(const BlockMap& blockMap, const Shape& shape, int x, int rotation);
    int calculateScore(const BlockMap& blockMap);
};

#endif // TETRISAI_H
