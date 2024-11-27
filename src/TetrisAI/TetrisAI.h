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
        bool rotateClockwise;
    };

    // Constructor and Destructor
    TetrisAI();
    ~TetrisAI();

    void setWeights(float heightWeight, float holeWeight, float bumpinessWeight, float lineClearWeight);
    void printSimulationResults(int score, int linesCleared);

    // Method to find the best move given the current state of the game
    Move findBestMove(const BlockMap& blockMap, const Shape& shape);


private:
    float heightExponent = 1.0;       // Default exponent for height scoring
    float holeUnderExponent = 2.0;   // Exponent for holes below filled blocks
    float holeLeftExponent = 1.5;    // Exponent for holes left of blocks
    float holeRightExponent = 1.5;   // Exponent for holes right of blocks

    // Heuristic weights (can be adjusted for different AI behavior)
    double lineClearWeight;
    double heightWeight;
    double holeWeight;
    double bumpinessWeight;

    void adjustHeuristicWeights(const BlockMap& blockMap); // Dynamically adjust weights
    double evaluatePlacement(const BlockMap& blockMap, const Shape& shape, int x, int rotation);
    double calculateScore(const BlockMap& blockMap);
};

#endif // TETRISAI_H
