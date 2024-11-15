#include "Game/Game.h"
#include "Config.h"
#include "ShapeFactory.h"
#include "TetrisAI/TetrisAI.h"

Game::Game(TFT_eSPI& tft, DisplayManager& displayManager, HighScoreManager& highScoreManager, InputHandler& inputHandler)
    : tft(tft), displayManager(displayManager), highScoreManager(highScoreManager), inputHandler(inputHandler),
      lastMoveDownTime(0), lastAIMoveTime(0), shape(nullptr), score(0), level(1), linesCleared(0), leftButtonState(IDLE), rightButtonState(IDLE),
      targetMoveSet(false), ai() // Initialize AI and move tracking
{}

void Game::setup() {
    displayStartScreenLoop();
    resetGame(); // Only reset and start the game after button press
}

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
            break; // Start the game
        }
    }
}

void Game::handleHighScoreDisplay() {
    inputHandler.waitForButtonRelease(BUTTON_LEFT);
    displayManager.clearScreen();
    displayManager.drawScreen();
    highScoreManager.displayHighScores(tft);
    displayManager.displayNavigation("     ", "Back");
    inputHandler.waitForButtonClick(BUTTON_RIGHT);
    inputHandler.waitForButtonRelease(BUTTON_RIGHT);
    displayManager.clearScreen();
    displayManager.drawScreen();
    displayManager.displayStartScreen();
    displayManager.displayNavigation("Highscores", "Start");
}

void Game::loop() {
    unsigned long currentTime = millis();

    if (blockMap.checkGameOver()) {
        handleGameOver();
        return;
    }

    handleShapeMovement(currentTime);

    if (shape) {
        updateShapePosition(currentTime);
    } else {
        createNewShape();
    }

    blockMap.drawAllBlocks(tft, BOX_SIZE);
}

void Game::handleShapeMovement(unsigned long currentTime) {
    // Ensure shape is valid before proceeding
    if (!shape) {
        Serial.println("Warning: handleShapeMovement called with a null shape.");
        return;
    }

    // AI determines best move when a new shape is created
    if (!targetMoveSet) {
        Serial.println("AI calculating best move...");
        bestMove = ai.findBestMove(blockMap, *shape);
        targetMoveSet = true; // Mark target as set to prevent repeated calculations
        Serial.printf("AI Best Move - X: %d, Rotation: %d\n", bestMove.x, bestMove.rotation);
    }

    // Move towards the AI's best move at regular intervals
    if (currentTime - lastAIMoveTime >= aiMoveInterval) {
        lastAIMoveTime = currentTime;

        // Check for valid shape state before making moves
        if (!shape) {
            Serial.println("Warning: Shape became null unexpectedly.");
            return; // Shape might have been deleted by another part of the code
        }

        // Handle AI rotations
        if (shape->getRotatePosition() != bestMove.rotation) {
            Serial.println("Rotating shape...");
            shape->eraseShape(tft, BOX_SIZE, displayManager.getBackgroundColor());
            shape->rotateClockwise(blockMap);
            shape->drawShape(tft, BOX_SIZE);
            Serial.println("Shape rotated.");
        } 
        // Handle horizontal movement
        else if (shape->getXPosition(0) < bestMove.x) {
            if (shape->isMovableToTheRight(blockMap)) { // Ensure movement is valid
                Serial.println("Moving shape right...");
                shape->eraseShape(tft, BOX_SIZE, displayManager.getBackgroundColor());
                shape->moveRight(blockMap);
                shape->drawShape(tft, BOX_SIZE);
                Serial.println("Shape moved right.");
            } else {
                Serial.println("Shape cannot move right (blocked or boundary).");
            }
        } else if (shape->getXPosition(0) > bestMove.x) {
            if (shape->isMovableToTheLeft(blockMap)) { // Ensure movement is valid
                Serial.println("Moving shape left...");
                shape->eraseShape(tft, BOX_SIZE, displayManager.getBackgroundColor());
                shape->moveLeft(blockMap);
                shape->drawShape(tft, BOX_SIZE);
                Serial.println("Shape moved left.");
            } else {
                Serial.println("Shape cannot move left (blocked or boundary).");
            }
        }
    }
}

void Game::updateShapePosition(unsigned long currentTime) {
    if (currentTime - lastMoveDownTime >= displayManager.getMoveDownSpeed(level)) {
        lastMoveDownTime = currentTime;
        if (shape->isMovableDownWards(blockMap)) {
            Serial.println("Moving shape down...");
            shape->eraseShape(tft, BOX_SIZE, displayManager.getBackgroundColor());
            shape->moveDown(blockMap);
            shape->drawShape(tft, BOX_SIZE);
            Serial.println("Shape moved down.");
        } else {
            Serial.println("Shape reached bottom or obstacle, adding to blockMap...");
            blockMap.addBlocks(shape->getBlockList(), 4);
            int clearedLines = blockMap.clearAndMoveAllFullLines(tft, BOX_SIZE, displayManager.getBackgroundColor());
            if (clearedLines > 0) {
                Serial.printf("Lines cleared: %d\n", clearedLines);
                updateScoreAndLevel(clearedLines);
            }
            delete shape;
            shape = nullptr;
            targetMoveSet = false;  // Reset target for the new shape
            Serial.println("Shape added to blockMap and deleted.");
        }
    }
}

void Game::createNewShape() {
    Serial.println("Creating new shape...");
    shape = ShapeFactory::createRandomShape();
    if (shape) {
        shape->moveToLowestBlockkAtMinusOne();
        shape->drawShape(tft, BOX_SIZE);
        targetMoveSet = false;  // AI will calculate moves for the new shape
        Serial.println("New shape created and positioned.");
    } else {
        Serial.println("Error: Failed to create new shape.");
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
