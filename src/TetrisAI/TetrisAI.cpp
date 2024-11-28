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
void TetrisAI::setWeights(float heightWeight, float maxHeightWeight, float holeWeight, float bumpinessWeight, float lineClearWeight) {
    this->heightWeight = heightWeight;
    this->maxHeightWeight = maxHeightWeight;
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
    int totalRotations = shape.getNumRotations();

    for (int targetRotation = 0; targetRotation < totalRotations; ++targetRotation) {
        Shape simulatedShape = shape;

        // Normalize rotations
        int clockwiseSteps = (targetRotation - currentRotation + totalRotations) % totalRotations;
        int anticlockwiseSteps = (currentRotation - targetRotation + totalRotations) % totalRotations;
        bool rotateClockwise = clockwiseSteps <= anticlockwiseSteps;

        int stepsToRotate = rotateClockwise ? clockwiseSteps : anticlockwiseSteps;

        // Apply the chosen rotation direction
        for (int i = 0; i < stepsToRotate; ++i) {
            if (rotateClockwise) {
                if (!simulatedShape.rotateClockwise(blockMap)) {
                    //Serial.println("Debug: Clockwise rotation blocked.");
                    break;
                }
            } else {
                if (!simulatedShape.rotateAntiClockwise(blockMap)) {
                    //Serial.println("Debug: Anti-clockwise rotation blocked.");
                    break;
                }
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

            double linesCleared = simulatedMap.clearAndMoveAllFullLines2(testShape);
            double score = calculateScore(simulatedMap, linesCleared);

            if (score > bestMove.score) {
                bestMove = {x, targetRotation, score, rotateClockwise};
            }
        }
    }

    // Debug: Log the best move
    //Serial.printf("Debug: BestMove -> X=%d, Rotation=%d, Score=%.2f, RotateClockwise=%s\n",
    //              bestMove.x, bestMove.rotation, bestMove.score,
    //              bestMove.rotateClockwise ? "true" : "false");

    return bestMove;
}


double TetrisAI::calculateScore(const BlockMap& blockMap, double linesCleared) {
    double totalHeight = 0.0;
    double holes = 0.0;
    double bumpiness = 0.0;
    double clearedLines = linesCleared;
    int previousHeight = 0;

    for (int x = 0; x < BlockMap::MAP_WIDTH; ++x) {
        int columnHeight = blockMap.getColumnHeight(x);
        totalHeight += pow(columnHeight, heightExponent);

        for (int y = BlockMap::MAP_HEIGHT - columnHeight; y < BlockMap::MAP_HEIGHT; ++y) {
            if (blockMap.getBlock(x, y) == nullptr) {
                // Left-neighbor holes
                if (x > 0 && blockMap.getBlock(x - 1, y) != nullptr) {
                    holes += pow(BlockMap::MAP_HEIGHT - y, holeLeftExponent);
                }

                // Right-neighbor holes
                if (x < BlockMap::MAP_WIDTH - 1 && blockMap.getBlock(x + 1, y) != nullptr) {
                    holes += pow(BlockMap::MAP_HEIGHT - y, holeRightExponent);
                }

                // Under-topmost holes
                if (y > 0 && blockMap.getBlock(x, y - 1) != nullptr) {
                    holes += pow(BlockMap::MAP_HEIGHT - y, holeUnderExponent);
                }
            }
        }

        // Bumpiness
        if (x > 0) {
            bumpiness += abs(columnHeight - previousHeight);
        }
        previousHeight = columnHeight;
    }

    return (clearedLines * lineClearWeight) +
           (totalHeight * heightWeight) +
           (holes * holeWeight) +
           (bumpiness * bumpinessWeight);
}
