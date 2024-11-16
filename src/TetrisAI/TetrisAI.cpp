#include "TetrisAI/TetrisAI.h"
#include "Game/BlockMap.h"
#include "Model/Shapes/Shape.h"
#include <limits>
#include "Config.h"

// Constructor
TetrisAI::TetrisAI()
    : heightWeight(1), holeWeight(5), bumpinessWeight(3), lineClearWeight(10) // Example heuristic weights
{}

// Destructor
TetrisAI::~TetrisAI() {}

// Method to find the best move based on the given state of the block map and shape
TetrisAI::Move TetrisAI::findBestMove(const BlockMap& blockMap, const Shape& shape) {
    Move bestMove = {0, 0, std::numeric_limits<int>::min()}; // Initialize with the worst score

    // Try all possible x positions and rotations for the shape
    for (int rotation = 0; rotation < 4; ++rotation) {
        for (int x = 0; x < BlockMap::MAP_WIDTH; ++x) {
            // Simulate placing the shape at this position and rotation
            int score = evaluatePlacement(blockMap, shape, x, rotation);
            // Update the best move if this move has a better score
            if (score > bestMove.score) {
                bestMove = {x, rotation, score};
            }
        }
    }

    return bestMove;
}

// Evaluates the score of placing the shape at the given position and rotation
int TetrisAI::evaluatePlacement(const BlockMap& blockMap, const Shape& shape, int x, int rotation) {
    // Create a copy of the block map for simulation
    BlockMap simulatedMap = blockMap;
    // Create a copy of the shape for simulation
    Shape simulatedShape = shape;

    // Apply the specified rotations
    for (int i = 0; i < rotation; ++i) {
        simulatedShape.rotateClockwise(simulatedMap);
    }
    // Set the shape to the specified x position
    simulatedShape.setPosition(x, 0);

    // Move the shape down to its final resting position
    while (simulatedShape.isMovableDownWards(simulatedMap)) {
        simulatedShape.moveDown(simulatedMap);
    }

    // Add the shape's blocks to the simulated block map
    simulatedMap.addBlocks(simulatedShape.getBlockList(), Shape::NUM_BLOCKS);

    // Calculate and return a score for the resulting state
    return calculateScore(simulatedMap);
}

// Calculates the heuristic score for the current state of the block map
int TetrisAI::calculateScore(const BlockMap& blockMap) {
    int height = blockMap.getColumnHeight(0); // Example: You might want to average across all columns instead
    int totalHoles = blockMap.getTotalHoles();
    int bumpiness = blockMap.getBumpiness();
    int clearedLines = blockMap.getAmoutOfFullLines(); // Simulate clearing lines without modifying the real state

    // Heuristic score: higher scores are better
    return (clearedLines * lineClearWeight) - (height * heightWeight + totalHoles * holeWeight + bumpiness * bumpinessWeight);
}
