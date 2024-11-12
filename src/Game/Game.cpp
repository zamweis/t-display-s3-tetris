#include "Game/Game.h"
#include "Config.h"

Game::Game(TFT_eSPI& tft, DisplayManager& displayManager, HighScoreManager& highScoreManager, InputHandler& inputHandler)
    : tft(tft), displayManager(displayManager), highScoreManager(highScoreManager), inputHandler(inputHandler),
      lastMoveDownTime(0), shape(nullptr), score(0), level(1), linesCleared(0), leftButtonState(IDLE), rightButtonState(IDLE) {}

void Game::setup() {
    tft.fillScreen(TFT_BLACK);
    displayManager.drawScreen();
    displayManager.drawCenteredText(false, score); // Display the start screen text
    
    // Wait for the player to press the button before starting
    inputHandler.waitForButtonClick(BUTTON_RIGHT); 
    resetGame(); // Only reset and start the game after button press
}

void Game::loop() {
    unsigned long currentTime = millis();
    
    // Check if game is over
    if (blockMap.checkGameOver()) {
        handleGameOver();
        return;
    }

    // Handle shape movement and button state
    handleButtonState(leftButtonState, BUTTON_LEFT, currentTime, &Shape::moveLeft, &Shape::rotateAntiClockwise);
    handleButtonState(rightButtonState, BUTTON_RIGHT, currentTime, &Shape::moveRight, &Shape::rotateClockwise);

    // Handle shape falling logic
    if (shape) {
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
    } else {
        shape = ShapeFactory::createRandomShape();
        if (shape) {
            shape->moveToLowestBlockkAtMinusOne();
            shape->drawShape(tft, BOX_SIZE);
        }
    }

    // Draw all blocks on the screen
    blockMap.drawAllBlocks(tft, BOX_SIZE);
}

void Game::resetGame() {
    score = 0;
    level = 1;
    linesCleared = 0;
    delete shape;
    shape = nullptr;
    blockMap = BlockMap();
    displayManager.drawGrid();
    shape = ShapeFactory::createRandomShape();
    if (shape) {
        shape->moveToLowestBlockkAtMinusOne();
        shape->drawShape(tft, BOX_SIZE);
    }
}

void Game::handleGameOver() {
    displayManager.drawScreen();
    displayManager.drawCenteredText(true, score); // Display "Game Over" screen

    // Wait for button press to go back to the start screen
    inputHandler.waitForButtonClick(BUTTON_RIGHT);

    if (highScoreManager.isHighScore(score)) {
        displayManager.clearScreen();
        displayManager.drawScreen();
        if (displayManager.promptPlayerForName()) {
            highScoreManager.updateHighScores(score, displayManager.getPlayerName());
        }
    }

    displayManager.drawScreen();
    highScoreManager.displayHighScores(tft);

    // Wait for button press to return to the start screen
    inputHandler.waitForButtonClick(BUTTON_RIGHT);
    displayManager.clearScreen();
    displayManager.drawScreen();
    displayManager.drawCenteredText(false, score); // Show start screen again
    inputHandler.waitForButtonClick(BUTTON_RIGHT); // Wait for button press to start a new game

    resetGame(); // Reset game state after returning to start screen
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
