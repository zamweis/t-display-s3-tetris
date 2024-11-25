#include "TetrisAI/TetrisAI.h"
#include "Game/BlockMap.h"
#include "Model/Shapes/Shape.h"
#include <limits>
#include "Config.h"

// Constructor with optimized weights
TetrisAI::TetrisAI()
    : lineClearWeight(10.0),  // Strong reward for clearing lines
      heightWeight(-4.0),    // Penalize high stacks more aggressively
      holeWeight(-6.0),      // Heavily penalize holes
      bumpinessWeight(-3.0)  // Penalize surface unevenness
{}

// Destructor
TetrisAI::~TetrisAI() {}

// Method to set weights
void TetrisAI::setWeights(float heightWeight, float holeWeight, float bumpinessWeight, float lineClearWeight) {
    this->heightWeight = heightWeight;
    this->holeWeight = holeWeight;
    this->bumpinessWeight = bumpinessWeight;
    this->lineClearWeight = lineClearWeight;
}

// Print results in CSV format for datasets
void TetrisAI::printSimulationResults(int score, int linesCleared) {
    Serial.printf("%.2f,%.2f,%.2f,%.2f,%d,%d\n",
                  heightWeight,
                  holeWeight,
                  bumpinessWeight,
                  lineClearWeight,
                  score,
                  linesCleared);
}

// Find the best move using heuristic evaluation
TetrisAI::Move TetrisAI::findBestMove(const BlockMap& blockMap, const Shape& shape) {
    Move bestMove = {0, 0, std::numeric_limits<double>::lowest()};
    int currentRotation = shape.getRotatePosition(); // Get the current rotation of the shape

    for (int rotation = 0; rotation < 4; ++rotation) {
        Shape simulatedShape = shape;

        // Calculate relative rotation steps
        int rotationSteps = (rotation - currentRotation + 4) % 4;

        for (int i = 0; i < rotationSteps; ++i) {
            if (!simulatedShape.isRotatableClockwise(blockMap)) break;
            simulatedShape.rotateClockwise(blockMap);
        }

        int shapeMinX = simulatedShape.getLeftBlock().getX();
        int shapeMaxX = simulatedShape.getRightBlock().getX();
        int mainBlockX = simulatedShape.getBlock(0).getX();

        int minX = mainBlockX - shapeMinX;
        int maxX = MAP_WIDTH - (shapeMaxX - mainBlockX) - 1;

        for (int x = minX; x <= maxX; ++x) {
            if (!simulatedShape.canMoveToPosition(x, simulatedShape.getBlock(0).getY(), blockMap)) continue;

            BlockMap simulatedMap = blockMap;
            simulatedShape.setPosition(x, simulatedShape.getBlock(0).getY());
            simulatedShape.fallDown(simulatedMap);
            simulatedMap.addBlocks(simulatedShape.getBlockList(), Shape::NUM_BLOCKS);

            double score = calculateScore(simulatedMap);

            if (score > bestMove.score) {
                bestMove = {x, rotation, score};
            }
        }
    }

    return bestMove;
}

// Heuristic evaluation score calculation
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