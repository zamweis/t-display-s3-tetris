#include "Game/Game.h"
#include "Config.h"

Game::Game(TFT_eSPI& tft, DisplayManager& displayManager, HighScoreManager& highScoreManager, InputHandler& inputHandler)
    : tft(tft), displayManager(displayManager), highScoreManager(highScoreManager), inputHandler(inputHandler),
      lastMoveDownTime(0), shape(nullptr), score(0), level(1), linesCleared(0), leftButtonState(IDLE), rightButtonState(IDLE) {}

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
    inputHandler.waitForButtonClick(BUTTON_RIGHT); // Wait for a button click to return to start screen
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

    displayHighScoreAndStartScreenLoop();
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

void Game::displayHighScoreAndStartScreenLoop() {
    while (true) {
        displayManager.drawScreen();
        highScoreManager.displayHighScores(tft);
        displayManager.displayNavigation("     ", "Next");
        inputHandler.waitForButtonClick(BUTTON_RIGHT);

        displayManager.clearScreen();
        displayManager.drawScreen();
        displayManager.displayStartScreen();
        displayManager.displayNavigation("Highscores", "Start");

        if (digitalRead(BUTTON_LEFT) == LOW) {
            inputHandler.waitForButtonRelease(BUTTON_LEFT);
            handleHighScoreDisplay();
        } else if (digitalRead(BUTTON_RIGHT) == LOW) {
            inputHandler.waitForButtonRelease(BUTTON_RIGHT);
            break; // Start the game
        }
    }
    resetGame(); // Only reset and start the game after button press
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
