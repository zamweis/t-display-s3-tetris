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

TetrisAI::Move TetrisAI::findBestMove(const BlockMap& blockMap, const Shape& shape) {
    Move bestMove = {0, 0, std::numeric_limits<int>::min()};
    Serial.println("AI: Beginne Berechnung des besten Zugs");

    BlockMap simulatedMap = blockMap;

    for (int rotation = 0; rotation < 4; ++rotation) {
        Shape simulatedShape = shape;

        for (int i = 0; i < rotation; ++i) {
            if (!simulatedShape.canRotateToPosition(simulatedShape.getRotatePosition() + 1, simulatedMap)) {
                break;
            }
            simulatedShape.rotateClockwise(simulatedMap);
        }

        for (int x = 0; x < BlockMap::MAP_WIDTH; ++x) {
            if (!simulatedShape.canMoveToPosition(x, 0, simulatedMap)) {
                continue;
            }

            simulatedShape.setPosition(x, 0);
            simulatedShape.fallDown(simulatedMap);

            int score = evaluatePlacement(simulatedMap, simulatedShape, x, rotation);
            Serial.printf("AI: Position (%d, %d) mit Score %d getestet\n", x, rotation, score);

            if (score > bestMove.score) {
                bestMove = {x, rotation, score};
                Serial.printf("AI: Neuer bester Zug gefunden: X=%d, Rotation=%d, Score=%d\n",
                              bestMove.x, bestMove.rotation, bestMove.score);
            }
        }
    }

    Serial.printf("AI: Bester Zug: X=%d, Rotation=%d, Score=%d\n", bestMove.x, bestMove.rotation, bestMove.score);
    return bestMove;
}

int TetrisAI::evaluatePlacement(const BlockMap& blockMap, const Shape& shape, int x, int rotation) {
    BlockMap simulatedMap = blockMap;
    Shape simulatedShape = shape;

    // Simuliere Rotation
    for (int i = 0; i < rotation; ++i) {
        simulatedShape.rotateClockwise(simulatedMap);
    }

    // Simuliere Bewegung
    simulatedShape.setPosition(x, 0);
    simulatedShape.fallDown(simulatedMap);

    // Füge die Form zur simulierten Map hinzu
    simulatedMap.addBlocks(simulatedShape.getBlockList(), Shape::NUM_BLOCKS);

    // Berechne den Score der simulierten Map
    return calculateScore(simulatedMap);
}

int TetrisAI::calculateScore(const BlockMap& blockMap) {
    int totalHeight = 0;
    for (int x = 0; x < BlockMap::MAP_WIDTH; ++x) {
        totalHeight += blockMap.getColumnHeight(x);
    }
    int avgHeight = totalHeight / BlockMap::MAP_WIDTH;

    int totalHoles = blockMap.getTotalHoles();
    int bumpiness = blockMap.getBumpiness();
    int clearedLines = blockMap.getAmoutOfFullLines();

    // Heuristik
    return (clearedLines * lineClearWeight) -
           (avgHeight * heightWeight + totalHoles * holeWeight + bumpiness * bumpinessWeight);
}