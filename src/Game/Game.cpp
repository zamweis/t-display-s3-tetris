#include "Game/Game.h"
#include "Config.h"
#include "TetrisAI/TetrisAI.h"
#include "ShapeFactory.h"

TetrisAI::Move currentMove;
bool isGravityActive = false;

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
    //handleShapeMovement(currentTime);

    if (blockMap.checkGameOver()) {
        Serial.println("Game Over erkannt.");
        handleGameOver();
        return;
    }

    if (shape) {
        static unsigned long lastAIStepTime = 0;
        unsigned long aiStepInterval = 200;

        if (currentTime - lastAIStepTime >= aiStepInterval) {
            if (!isGravityActive) {
                executeAIStep();
                lastAIStepTime = currentTime;
            }
        }
        if (shape) {
            // Gravity and shape deletion
            updateShapePosition(currentTime);
        }
    } else {
        createNewShape();
    }

    blockMap.drawAllBlocks(tft);
}

void Game::handleShapeMovement(unsigned long currentTime) {
    handleButtonState(leftButtonState, BUTTON_LEFT, currentTime, &Shape::moveLeft, &Shape::rotateAntiClockwise);
    handleButtonState(rightButtonState, BUTTON_RIGHT, currentTime, &Shape::moveRight, &Shape::rotateClockwise);
}

void Game::updateShapePosition(unsigned long currentTime) {
    if (currentTime - lastMoveDownTime >= displayManager.getMoveDownSpeed(level)) {
        isGravityActive = true; // Gravitation aktiv
        lastMoveDownTime = currentTime;

        if (shape->isMovableDownWards(blockMap)) {
            shape->eraseShape(tft, displayManager.getBackgroundColor());
            shape->moveDown(blockMap);
            shape->drawShape(tft);
        } else {
            blockMap.addBlocks(shape->getBlockList(), 4);
            int clearedLines = blockMap.clearAndMoveAllFullLines(tft, displayManager.getBackgroundColor());
            if (clearedLines > 0) updateScoreAndLevel(clearedLines);
            delete shape;
            shape = nullptr;
            currentMove.score = std::numeric_limits<int>::min(); // AI-Zug zurücksetzen
        }

        isGravityActive = false; // Gravitation abgeschlossen
    }
}

void Game::createNewShape() {
    shape = ShapeFactory::createRandomShape();
    if (shape) {
        if (!shape->canMoveToPosition(shape->getBlock(0).getX(), shape->getBlock(0).getY(), blockMap)) {
            //Serial.println("Game Over: Keine gültige Startposition für neue Shape.");
            delete shape;
            shape = nullptr;
            return;
        }

        //Serial.println("Neue Shape erstellt und erfolgreich positioniert.");
        shape->moveToLowestBlockkAtMinusOne();
        shape->drawShape(tft);
    } else {
        Serial.println("Fehler: Shape konnte nicht erstellt werden.");
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
        //Serial.printf("Score: %d, Level: %d\n", score, level);
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
                shape->eraseShape(tft, displayManager.getBackgroundColor());
                (shape->*moveFunc)(blockMap);
                shape->drawShape(tft);
            }
            state = IDLE;
            break;
        case LONG_PRESS:
            if (inputHandler.isButtonReleased(buttonPin)) {
                state = IDLE;
            } else {
                if (currentTime - inputHandler.getLastRotateTime() >= rotationDebounceInterval) {
                    if (shape) {
                        shape->eraseShape(tft, displayManager.getBackgroundColor());
                        (shape->*rotateFunc)(blockMap);
                        shape->drawShape(tft);
                        inputHandler.updateLastRotateTime(currentTime);
                    }
                }
            }
            break;
    }
}

bool Game::executeAIStep() {
    if (!shape) {
        Serial.println("No shape available. AI step aborted.");
        return false;
    }

    if (isGravityActive) {
        Serial.println("AI paused: Gravity active.");
        return false; // AI waits until gravity operation is complete
    }

    // If no current move, calculate the best move
    if (currentMove.score == std::numeric_limits<int>::min()) {
        currentMove = tetrisAI.findBestMove(blockMap, *shape);

        if (currentMove.score == std::numeric_limits<int>::min()) {
            Serial.println("AI could not find a valid move.");
            return false;
        }

        // Validate the AI move's bounds
        if (currentMove.x < 0 || currentMove.x >= BlockMap::MAP_WIDTH) {
            Serial.printf("AI move out of bounds: Invalid X=%d.\n", currentMove.x);
            return false;
        }
        if (currentMove.rotation < 0 || currentMove.rotation > 3) {
            Serial.printf("AI move out of bounds: Invalid rotation=%d.\n", currentMove.rotation);
            return false;
        }
    }

    // Stepwise rotation (ensure rotation before moving horizontally)
    if (shape->getRotatePosition() != currentMove.rotation) {
        int nextRotation = shape->getRotatePosition() < currentMove.rotation
                               ? shape->getRotatePosition() + 1
                               : shape->getRotatePosition() - 1;

        if (shape->canRotateToPosition(nextRotation, blockMap)) {
            shape->eraseShape(tft, displayManager.getBackgroundColor());
            shape->rotateToPosition(nextRotation, blockMap);
            shape->drawShape(tft);
            return true; // Rotation performed, no need to move horizontally yet
        }
    }

    // Horizontal movement after rotation is complete
    if (shape->getBlock(0).getX() != currentMove.x) {
        if (shape->getBlock(0).getX() < currentMove.x && shape->isMovableToTheRight(blockMap)) {
            shape->eraseShape(tft, displayManager.getBackgroundColor());
            shape->moveRight(blockMap);
            shape->drawShape(tft);
            return true; // Movement to the right performed
        } else if (shape->getBlock(0).getX() > currentMove.x && shape->isMovableToTheLeft(blockMap)) {
            shape->eraseShape(tft, displayManager.getBackgroundColor());
            shape->moveLeft(blockMap);
            shape->drawShape(tft);
            return true; // Movement to the left performed
        }
    }

    // If the target position is reached, drop the shape
    if (shape->getBlock(0).getX() == currentMove.x && shape->getRotatePosition() == currentMove.rotation) {
        shape->eraseShape(tft, displayManager.getBackgroundColor());
        shape->fallDown(blockMap);
        blockMap.addBlocks(shape->getBlockList(), Shape::NUM_BLOCKS);
        shape->drawShape(tft);
        delete shape;
        shape = nullptr;
        currentMove.score = std::numeric_limits<int>::min(); // Reset AI move
        return true;
    }

    return false;
}
