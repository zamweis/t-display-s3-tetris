#include "Game/Game.h"
#include "Config.h"
#include "TetrisAI/TetrisAI.h" // Include TetrisAI header
#include "ShapeFactory.h"

Game::Game(TFT_eSPI& tft, DisplayManager& displayManager, HighScoreManager& highScoreManager, InputHandler& inputHandler)
    : tft(tft), displayManager(displayManager), highScoreManager(highScoreManager), inputHandler(inputHandler),
      lastMoveDownTime(0), shape(nullptr), score(0), level(1), linesCleared(0), leftButtonState(IDLE), rightButtonState(IDLE),
      tetrisAI() {} // Initialize TetrisAI instance

void Game::displayStartScreenLoop() {
    tft.fillScreen(TFT_BLACK);
    displayManager.drawScreen();
    displayManager.displayStartScreen();
    displayManager.displayNavigation("Highscores", "Start");
    highScoreManager.loadHighScores();

    while (true) {
        if (digitalRead(BUTTON_LEFT) == LOW) {
            handleHighScoreDisplay();
        } else if (digitalRead(BUTTON_RIGHT) == LOW) {
            inputHandler.waitForButtonRelease(BUTTON_RIGHT);
            break; // Exit loop and start the game
        }
    }
}

void Game::handleHighScoreDisplay() {
    inputHandler.waitForButtonRelease(BUTTON_LEFT);
    displayManager.clearScreen();
    displayManager.drawScreen();
    highScoreManager.displayHighScores(tft);
    displayManager.displayNavigation("     ", "Back");
    inputHandler.waitForButtonClick(BUTTON_RIGHT); // Wait for a button click to return to start screen
    inputHandler.waitForButtonRelease(BUTTON_RIGHT);
    displayManager.clearScreen();
    displayManager.drawScreen();
    displayManager.displayStartScreen();
    displayManager.displayNavigation("Highscores", "Start");
}

void Game::setup() {
    displayStartScreenLoop();
    resetGame(); // Only reset and start the game after button press
}

void Game::loop() {
    unsigned long currentTime = millis();

    if (blockMap.checkGameOver()) {
        handleGameOver();
        return;
    }

    // Schrittweise AI-Steuerung
    if (shape) {
        executeAIStep();
        delay(200); // Füge eine Pause hinzu, um die Animation sichtbar zu machen
    } else {
        createNewShape();
    }

    blockMap.drawAllBlocks(tft, BOX_SIZE);
}

void Game::executeAIMove() {
    if (!shape) return; // Ensure there is a shape

    // Find the best move using TetrisAI
    TetrisAI::Move bestMove = tetrisAI.findBestMove(blockMap, *shape);

    // Apply the best move
    for (int i = 0; i < bestMove.rotation; ++i) {
        shape->rotateClockwise(blockMap);
    }
    shape->setPosition(bestMove.x, 0); // Move shape to the suggested x position
}

void Game::handleShapeMovement(unsigned long currentTime) {
    handleButtonState(leftButtonState, BUTTON_LEFT, currentTime, &Shape::moveLeft, &Shape::rotateAntiClockwise);
    handleButtonState(rightButtonState, BUTTON_RIGHT, currentTime, &Shape::moveRight, &Shape::rotateClockwise);
}

void Game::updateShapePosition(unsigned long currentTime) {
    if (currentTime - lastMoveDownTime >= displayManager.getMoveDownSpeed(level)) {
        lastMoveDownTime = currentTime;
        if (shape->isMovableDownWards(blockMap)) {
            shape->eraseShape(tft, BOX_SIZE, displayManager.getBackgroundColor());
            shape->moveDown(blockMap);
            shape->drawShape(tft, BOX_SIZE);
        } else {
            blockMap.addBlocks(shape->getBlockList(), 4);
            int clearedLines = blockMap.clearAndMoveAllFullLines(tft, BOX_SIZE, displayManager.getBackgroundColor());
            if (clearedLines > 0) updateScoreAndLevel(clearedLines);
            delete shape;
            shape = nullptr;
        }
    }
}

void Game::createNewShape() {
    shape = ShapeFactory::createRandomShape();
    if (shape) {
        shape->moveToLowestBlockkAtMinusOne();
        shape->drawShape(tft, BOX_SIZE);
    }
}

void Game::resetGame() {
    score = 0;
    level = 1;
    linesCleared = 0;
    delete shape;
    shape = nullptr;
    blockMap = BlockMap();
    displayManager.drawGrid();
    createNewShape();
}

void Game::handleGameOver() {
    displayManager.drawScreen();
    displayManager.displayGameOverScreen(score);
    highScoreManager.loadHighScores();

    inputHandler.waitForButtonRelease(BUTTON_RIGHT);
    inputHandler.waitForButtonClick(BUTTON_RIGHT);

    if (highScoreManager.isHighScore(score)) {
        handleHighScoreEntry();
    }

    displayStartScreenLoop();
    resetGame(); // Only reset and start the game after button press
}

void Game::handleHighScoreEntry() {
    displayManager.clearScreen();
    displayManager.drawScreen();
    if (displayManager.promptPlayerForName()) {
        highScoreManager.updateHighScores(score, displayManager.getPlayerName());
        displayManager.resetNameEntry();
        inputHandler.waitForButtonRelease(BUTTON_RIGHT);
    }
}

void Game::updateScoreAndLevel(int clearedLines) {
    const int pointsPerLine[4] = {40, 100, 300, 1200};
    if (clearedLines > 0 && clearedLines <= 4) {
        score += pointsPerLine[clearedLines - 1] * (level + 1);
        linesCleared += clearedLines;
        level = 1 + linesCleared / 10;
        Serial.printf("Score: %d, Level: %d\n", score, level);
    }
}

void Game::handleButtonState(ButtonState &state, int buttonPin, unsigned long currentTime, void (Shape::*moveFunc)(BlockMap &), void (Shape::*rotateFunc)(BlockMap &)) {
    switch (state) {
        case IDLE:
            if (inputHandler.isButtonPressed(buttonPin)) {
                state = PRESSED;
                inputHandler.startButtonPressTimer(currentTime);
            }
            break;
        case PRESSED:
            if (inputHandler.isButtonReleased(buttonPin)) {
                state = (currentTime - inputHandler.getButtonPressStart() < rotationHoldThreshold) ? SHORT_PRESS : IDLE;
            } else if (currentTime - inputHandler.getButtonPressStart() >= rotationHoldThreshold) {
                state = LONG_PRESS;
            }
            break;
        case SHORT_PRESS:
            if (shape) {
                shape->eraseShape(tft, BOX_SIZE, displayManager.getBackgroundColor());
                (shape->*moveFunc)(blockMap);
                shape->drawShape(tft, BOX_SIZE);
            }
            state = IDLE;
            break;
        case LONG_PRESS:
            if (inputHandler.isButtonReleased(buttonPin)) {
                state = IDLE;
            } else {
                if (currentTime - inputHandler.getLastRotateTime() >= rotationDebounceInterval) {
                    if (shape) {
                        shape->eraseShape(tft, BOX_SIZE, displayManager.getBackgroundColor());
                        (shape->*rotateFunc)(blockMap);
                        shape->drawShape(tft, BOX_SIZE);
                        inputHandler.updateLastRotateTime(currentTime);
                    }
                }
            }
            break;
    }
}

TetrisAI::Move currentMove; // Globale oder Klassenvariable, um den aktuellen Zug zu speichern

bool Game::executeAIStep() {
    if (!shape) return false;

    // Berechne neuen Zug, falls keiner vorhanden
    if (currentMove.score == std::numeric_limits<int>::min()) {
        currentMove = tetrisAI.findBestMove(blockMap, *shape);
        if (currentMove.score == std::numeric_limits<int>::min()) {
            Serial.println("AI konnte keinen gültigen Zug finden.");
            return false;
        }
    }

    // Schrittweise Rotation
    int nextRotation = shape->getRotatePosition();
    if (shape->getRotatePosition() < currentMove.rotation) {
        nextRotation = shape->getRotatePosition() + 1;
    } else if (shape->getRotatePosition() > currentMove.rotation) {
        nextRotation = shape->getRotatePosition() - 1;
    }

    if (nextRotation != shape->getRotatePosition() && shape->canRotateToPosition(nextRotation, blockMap)) {
        shape->rotateToPosition(nextRotation, blockMap);
        return true; // Rotation abgeschlossen
    }

    // Schrittweise Bewegung nach links oder rechts
    if (shape->getBlock(0).getX() < currentMove.x) {
        if (shape->isMovableToTheRight(blockMap)) {
            shape->moveRight(blockMap);
            return true; // Bewegung abgeschlossen
        }
    } else if (shape->getBlock(0).getX() > currentMove.x) {
        if (shape->isMovableToTheLeft(blockMap)) {
            shape->moveLeft(blockMap);
            return true; // Bewegung abgeschlossen
        }
    }

    // Wenn Rotation und Bewegung abgeschlossen sind, lasse die Form fallen
    if (shape->isMovableDownWards(blockMap)) {
        shape->moveDown(blockMap);
    } else {
        // Form platzieren und neuen Zug vorbereiten
        blockMap.addBlocks(shape->getBlockList(), Shape::NUM_BLOCKS);
        delete shape;
        shape = nullptr;
        currentMove.score = std::numeric_limits<int>::min();
    }

    return true;
}
