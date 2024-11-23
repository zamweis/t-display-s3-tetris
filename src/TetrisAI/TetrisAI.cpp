#include "TetrisAI/TetrisAI.h"
#include "Game/BlockMap.h"
#include "Model/Shapes/Shape.h"
#include <limits>
#include "Config.h"

// Constructor with refined weights
TetrisAI::TetrisAI()
    : lineClearWeight(0.76),  // Strong priority for line clears
      heightWeight(-0.51),    // Moderate penalty for height
      holeWeight(-0.66),      // Heavy penalty for holes
      bumpinessWeight(-0.18)  // Moderate penalty for bumpiness
{}

// Destructor
TetrisAI::~TetrisAI() {}

TetrisAI::Move TetrisAI::findBestMove(const BlockMap& blockMap, const Shape& shape) {
    Move bestMove = {0, 0, std::numeric_limits<double>::lowest()};
    BlockMap simulatedMap = blockMap;

    // Adjust heuristic weights dynamically based on the board state
    adjustHeuristicWeights(blockMap);

    for (int rotation = 0; rotation < 4; ++rotation) {
        Shape simulatedShape = shape;

        // Simulate rotation
        for (int i = 0; i < rotation; ++i) {
            if (!simulatedShape.canRotateToPosition(simulatedShape.getRotatePosition() + 1, simulatedMap)) {
                break;  // Skip invalid rotations
            }
            simulatedShape.rotateClockwise(simulatedMap);
        }

        // Test all horizontal placements
        for (int x = 0; x < BlockMap::MAP_WIDTH; ++x) {
            if (!simulatedShape.canMoveToPosition(x, 0, simulatedMap)) {
                continue;  // Skip invalid positions
            }

            // Simulate placement
            simulatedShape.setPosition(x, 0);
            simulatedShape.fallDown(simulatedMap);

            // Evaluate the score
            double score = evaluatePlacement(simulatedMap, simulatedShape, x, rotation);

            if (score > bestMove.score) {
                bestMove = {x, rotation, score};
            }
        }
    }

    Serial.printf("Best move: X=%d, Rotation=%d, Score=%.6f\n", bestMove.x, bestMove.rotation, bestMove.score);
    return bestMove;
}

void TetrisAI::adjustHeuristicWeights(const BlockMap& blockMap) {
    // Prioritize clearing lines and penalize height more as the game progresses
    int highestColumn = blockMap.getColumnHeight(0);
    if (highestColumn > BlockMap::MAP_HEIGHT / 2) {
        lineClearWeight += 0.2;  // Higher priority for clearing lines
        heightWeight -= 0.1;     // Increased penalty for height
    }
}

double TetrisAI::evaluatePlacement(const BlockMap& blockMap, const Shape& shape, int x, int rotation) {
    BlockMap simulatedMap = blockMap;
    Shape simulatedShape = shape;

    // Apply rotation
    for (int i = 0; i < rotation; ++i) {
        simulatedShape.rotateClockwise(simulatedMap);
    }

    // Simulate placement
    simulatedShape.setPosition(x, 0);
    simulatedShape.fallDown(simulatedMap);

    // Add the shape to the map
    simulatedMap.addBlocks(simulatedShape.getBlockList(), Shape::NUM_BLOCKS);

    // Calculate the score
    return calculateScore(simulatedMap);
}

double TetrisAI::calculateScore(const BlockMap& blockMap) {
    double totalHeight = 0.0;
    double holes = 0.0;
    double bumpiness = 0.0;
    double clearedLines = static_cast<double>(blockMap.getAmoutOfFullLines());

    for (int x = 0; x < BlockMap::MAP_WIDTH; ++x) {
        int columnHeight = blockMap.getColumnHeight(x);
        totalHeight += columnHeight;

        // Count holes
        bool blockFound = false;
        for (int y = BlockMap::MAP_HEIGHT - columnHeight; y < BlockMap::MAP_HEIGHT; ++y) {
            if (blockMap.getBlock(x, y) != nullptr) {
                blockFound = true;
            } else if (blockFound) {
                holes++;
            }
        }

        // Calculate bumpiness
        if (x > 0) {
            int prevHeight = blockMap.getColumnHeight(x - 1);
            bumpiness += abs(columnHeight - prevHeight);
        }
    }

    // Normalize total height to make weights scale-independent
    double normalizedHeight = totalHeight / BlockMap::MAP_WIDTH;

    // Heuristic score calculation
    return (clearedLines * lineClearWeight) +
           (normalizedHeight * heightWeight) +
           (holes * holeWeight) +
           (bumpiness * bumpinessWeight);
}
