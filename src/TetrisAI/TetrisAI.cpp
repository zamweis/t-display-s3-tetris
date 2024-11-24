#include "TetrisAI/TetrisAI.h"
#include "Game/BlockMap.h"
#include "Model/Shapes/Shape.h"
#include <limits>
#include "Config.h"

// Constructor with optimized weights
TetrisAI::TetrisAI()
    : lineClearWeight(10.0),   // Strong reward for clearing lines
      heightWeight(-4.0),     // Penalize high stacks more aggressively
      holeWeight(-6.0),       // Heavily penalize holes
      bumpinessWeight(-3.0)   // Penalize surface unevenness
{}

// Destructor
TetrisAI::~TetrisAI() {}

TetrisAI::Move TetrisAI::findBestMove(const BlockMap& blockMap, const Shape& shape) {
    Move bestMove = {0, 0, std::numeric_limits<double>::lowest()};

    for (int rotation = 0; rotation < 4; ++rotation) {
        Shape simulatedShape = shape;

        if (!simulatedShape.canRotateToPosition(rotation, blockMap, true) &&
            !simulatedShape.canRotateToPosition(rotation, blockMap, false)) {
            continue; // Skip invalid rotations
        }

        for (int i = 0; i < rotation; ++i) {
            simulatedShape.rotateClockwise(blockMap);
        }

        int shapeMinX = simulatedShape.getLeftBlock().getX();
        int shapeMaxX = simulatedShape.getRightBlock().getX();
        int mainBlockX = simulatedShape.getMaintBlock().getX();

        int minX = mainBlockX - shapeMinX;
        int maxX = MAP_WIDTH - (shapeMaxX - mainBlockX) - 1;

        for (int x = minX; x <= maxX; ++x) {
            if (!simulatedShape.canMoveToPosition(x, simulatedShape.getMaintBlock().getY(), blockMap)) {
                continue;
            }

            BlockMap simulatedMap = blockMap;
            simulatedShape.setPosition(x, simulatedShape.getMaintBlock().getY());
            simulatedShape.fallDown(simulatedMap);
            simulatedMap.addBlocks(simulatedShape.getBlockList(), Shape::NUM_BLOCKS);

            double score = calculateScore(simulatedMap);

            if (score > bestMove.score) {
                bestMove = {x, rotation, score};
            }
        }
    }

    if (bestMove.score == std::numeric_limits<double>::lowest()) {
        Serial.println("AI: No valid moves found.");
    } else {
        Serial.printf("AI: Best move finalized -> X=%d, Rotation=%d, Score=%.2f\n", bestMove.x, bestMove.rotation, bestMove.score);
    }

    return bestMove;
}

double TetrisAI::calculateScore(const BlockMap& blockMap) {
    double totalHeight = 0.0;
    double holes = 0.0;
    double bumpiness = 0.0;
    double clearedLines = static_cast<double>(blockMap.getAmoutOfFullLines());

    int previousHeight = 0;

    for (int x = 0; x < BlockMap::MAP_WIDTH; ++x) {
        int columnHeight = blockMap.getColumnHeight(x);
        totalHeight += columnHeight;

        for (int y = BlockMap::MAP_HEIGHT - columnHeight; y < BlockMap::MAP_HEIGHT; ++y) {
            if (blockMap.getBlock(x, y) == nullptr && blockMap.getBlock(x, y - 1) != nullptr) {
                holes++;
            }
        }

        if (x > 0) {
            bumpiness += abs(columnHeight - previousHeight);
        }

        previousHeight = columnHeight;
    }

    double normalizedHeight = totalHeight / BlockMap::MAP_WIDTH;

    return (clearedLines * lineClearWeight) +
           (normalizedHeight * heightWeight) +
           (holes * holeWeight) +
           (bumpiness * bumpinessWeight);
}
