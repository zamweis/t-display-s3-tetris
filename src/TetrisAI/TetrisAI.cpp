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

TetrisAI::Move TetrisAI::findBestMove(const BlockMap& blockMap, const Shape& shape) {
    Move bestMove = {0, 0, std::numeric_limits<double>::lowest(), true};
    int currentRotation = shape.getRotatePosition(); // Get the current rotation of the shape

    for (int targetRotation = 0; targetRotation < 4; ++targetRotation) {
        Shape simulatedShape = shape;

        // Determine the shortest rotation direction
        int clockwiseSteps = (targetRotation - currentRotation + 4) % 4;
        int anticlockwiseSteps = (currentRotation - targetRotation + 4) % 4;
        bool rotateClockwise = clockwiseSteps <= anticlockwiseSteps;

        int stepsToRotate = rotateClockwise ? clockwiseSteps : anticlockwiseSteps;

        // Apply the chosen rotation direction
        for (int i = 0; i < stepsToRotate; ++i) {
            if (rotateClockwise) {
                if (!simulatedShape.rotateClockwise(blockMap)) break;
            } else {
                if (!simulatedShape.rotateAntiClockwise(blockMap)) break;
            }
        }

        // Get horizontal bounds based on the current rotation and shape position
        auto [shapeMinX, shapeMaxX] = simulatedShape.getHorizontalBounds(blockMap);
        int mainBlockX = simulatedShape.getBlock(0).getX();

        int minX = mainBlockX - shapeMinX;
        int maxX = MAP_WIDTH - (shapeMaxX - mainBlockX) - 1;

        for (int x = minX; x <= maxX; ++x) {
            Shape testShape = simulatedShape; // Create a copy for movement simulation

            // Step-by-step horizontal movement to check for collisions
            bool canReach = true;
            int currentX = testShape.getBlock(0).getX();
            while (currentX != x) {
                if (currentX < x) {
                    if (!testShape.isMovableToTheRight(blockMap)) {
                        canReach = false;
                        break;
                    }
                    testShape.moveRight();
                    currentX++;
                } else {
                    if (!testShape.isMovableToTheLeft(blockMap)) {
                        canReach = false;
                        break;
                    }
                    testShape.moveLeft();
                    currentX--;
                }
            }

            if (!canReach) continue; // Skip if the target position is not reachable

            // Simulate the fall and evaluate the score
            BlockMap simulatedMap = blockMap;
            testShape.fallDown(simulatedMap);
            simulatedMap.addBlocks(testShape.getBlockList(), Shape::NUM_BLOCKS);

            double score = calculateScore(simulatedMap);

            if (score > bestMove.score) {
                bestMove = {x, targetRotation, score, rotateClockwise};
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