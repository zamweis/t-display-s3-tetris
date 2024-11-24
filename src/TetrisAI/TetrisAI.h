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
    double lineClearWeight;
    double heightWeight;
    double holeWeight;
    double bumpinessWeight;

    void adjustHeuristicWeights(const BlockMap& blockMap); // Dynamically adjust weights
    double evaluatePlacement(const BlockMap& blockMap, const Shape& shape, int x, int rotation);
    double calculateScore(const BlockMap& blockMap);
    double calculateConnectivity(const BlockMap& blockMap, const Shape& shape);
};

#endif // TETRISAI_H
