#include "TetrisAI.h"
#include <limits>

// Constructor
TetrisAI::TetrisAI() 
    : heightWeight(1), holeWeight(5), bumpinessWeight(3) // Example weights
{}

// Destructor
TetrisAI::~TetrisAI() {}

TetrisAI::Move TetrisAI::findBestMove(const BlockMap& blockMap, const Shape& shape) {
    Move bestMove = {0, 0, std::numeric_limits<int>::min()};

    // Try all possible x positions and rotations
    for (int rotation = 0; rotation < 4; ++rotation) {
        for (int x = 0; x < BlockMap::MAP_WIDTH; ++x) {
            int score = evaluatePlacement(blockMap, shape, x, rotation);
            if (score > bestMove.score) {
                bestMove = {x, rotation, score};
            }
        }
    }

    return bestMove;
}

int TetrisAI::evaluatePlacement(const BlockMap& blockMap, const Shape& shape, int x, int rotation) {
    BlockMap simulatedMap = simulatePlacement(blockMap, shape, x, rotation);
    return calculateScore(simulatedMap);
}

int TetrisAI::calculateScore(const BlockMap& blockMap) {
    int height = blockMap.calculateHeight();
    int holes = blockMap.countHoles();
    int bumpiness = blockMap.calculateBumpiness();
    int clearedLines = blockMap.countClearedLines();

    // Improved scoring calculation
    return (clearedLines * 10) - (height * heightWeight + holes * holeWeight + bumpiness * bumpinessWeight);
}

BlockMap TetrisAI::simulatePlacement(const BlockMap& blockMap, const Shape& shape, int x, int rotation) {
    BlockMap simulatedMap = blockMap;
    Shape simulatedShape = shape;

    for (int i = 0; i < rotation; ++i) {
        simulatedShape.rotateClockwise(simulatedMap);
    }
    simulatedShape.setPosition(x, 0);

    while (simulatedShape.isMovableDownWards(simulatedMap)) {
        simulatedShape.moveDown(simulatedMap);
    }

    simulatedMap.addBlocks(simulatedShape.getBlockList(), simulatedShape.NUM_BLOCKS);

    return simulatedMap;
}