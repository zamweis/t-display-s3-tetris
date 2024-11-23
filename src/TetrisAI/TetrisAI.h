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
        double score;
    };

    // Constructor and Destructor
    TetrisAI();
    ~TetrisAI();

    // Method to find the best move given the current state of the game
    Move findBestMove(const BlockMap& blockMap, const Shape& shape);


private:
    // Heuristic weights (can be adjusted for different AI behavior)
    double heightWeight;
    double holeWeight;
    double bumpinessWeight;
    double lineClearWeight;

    // Helper methods for evaluating and scoring potential moves
    double evaluatePlacement(const BlockMap& blockMap, const Shape& shape, int x, int rotation);
    double calculateScore(const BlockMap& blockMap);
    void adjustHeuristicWeights(const BlockMap& blockMap);
};

#endif // TETRISAI_H
