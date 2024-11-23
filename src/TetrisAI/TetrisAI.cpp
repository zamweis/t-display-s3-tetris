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

    // Erstelle eine Kopie der BlockMap
    BlockMap simulatedMap = blockMap;

    // Iteriere über alle möglichen Rotationen
    for (int rotation = 0; rotation < 4; ++rotation) {
        Shape simulatedShape = shape;

        // Simuliere die Rotation
        for (int i = 0; i < rotation; ++i) {
            if (!simulatedShape.canRotateToPosition(simulatedShape.getRotatePosition() + 1, simulatedMap)) {
                break; // Ungültige Rotation
            }
            simulatedShape.rotateClockwise(simulatedMap);
        }

        // Iteriere über alle möglichen x-Positionen
        for (int x = 0; x < BlockMap::MAP_WIDTH; ++x) {
            // Prüfe, ob die Form an die Position bewegt werden kann
            if (!simulatedShape.canMoveToPosition(x, 0, simulatedMap)) {
                continue; // Ungültige Position
            }

            // Simuliere die Bewegung
            simulatedShape.setPosition(x, 0);
            simulatedShape.fallDown(simulatedMap);

            // Berechne den Score
            int score = evaluatePlacement(simulatedMap, simulatedShape, x, rotation);

            // Aktualisiere den besten Zug
            if (score > bestMove.score) {
                bestMove = {x, rotation, score};
            }
        }
    }

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
    // Höhe
    int totalHeight = 0;
    for (int x = 0; x < BlockMap::MAP_WIDTH; ++x) {
        totalHeight += blockMap.getColumnHeight(x);
    }
    int avgHeight = totalHeight / BlockMap::MAP_WIDTH;

    // Löcher
    int totalHoles = blockMap.getTotalHoles();

    // Unebenheit
    int bumpiness = blockMap.getBumpiness();

    // Gelöschte Reihen
    int clearedLines = blockMap.getAmoutOfFullLines();

    // Heuristik
    return (clearedLines * lineClearWeight) -
           (avgHeight * heightWeight + totalHoles * holeWeight + bumpiness * bumpinessWeight);
}
