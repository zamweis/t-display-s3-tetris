#include "Game/Game.h"
#include "Config.h"

Game::Game(TFT_eSPI& tft, DisplayManager& displayManager, HighScoreManager& highScoreManager, InputHandler& inputHandler)
    : tft(tft), displayManager(displayManager), highScoreManager(highScoreManager), inputHandler(inputHandler),
      lastMoveDownTime(0), shape(nullptr), score(0), level(1), linesCleared(0), leftButtonState(IDLE), rightButtonState(IDLE) {}

void Game::setup() {
    tft.fillScreen(TFT_BLACK);
    displayManager.drawScreen();
    displayManager.displayStartScreen();
    displayManager.displayNavigation("Highscores", "Start");
    highScoreManager.loadHighScores();

    // Wait for the player to press either left or right button
    while (true) {
        if (digitalRead(BUTTON_LEFT) == LOW) {
            inputHandler.waitForButtonRelease(BUTTON_LEFT);
            // Display high scores when the left button is pressed
            displayManager.clearScreen();
            displayManager.drawScreen();
            highScoreManager.displayHighScores(tft);
            displayManager.displayNavigation("     ", "Back");
            inputHandler.waitForButtonClick(BUTTON_RIGHT); // Wait for a button click to return to start screen
            displayManager.clearScreen();
            displayManager.drawScreen();
            displayManager.displayStartScreen();
            displayManager.displayNavigation("Highscores", "Start");
        } else if (digitalRead(BUTTON_RIGHT) == LOW) {
            inputHandler.waitForButtonRelease(BUTTON_RIGHT);
            // Start the game when the right button is pressed
            break;
        }
    }

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
    displayManager.displayGameOverScreen(score); // Display "Game Over" screen
    highScoreManager.loadHighScores();

    // Wait for button click to go back to the start screen
    inputHandler.waitForButtonRelease(BUTTON_RIGHT);
    inputHandler.waitForButtonClick(BUTTON_RIGHT);

    // If highscore prompt player for name
    if (highScoreManager.isHighScore(score)) {
        displayManager.clearScreen();
        displayManager.drawScreen();
        if (displayManager.promptPlayerForName()) {
            highScoreManager.updateHighScores(score, displayManager.getPlayerName());
            displayManager.resetNameEntry();
            inputHandler.waitForButtonRelease(BUTTON_RIGHT);
        }
    }

    // Display HighScores
    displayManager.drawScreen();
    highScoreManager.displayHighScores(tft);
    displayManager.displayNavigation("     ", "Next");

    while (true) {
        if (digitalRead(BUTTON_LEFT) == LOW) {
            inputHandler.waitForButtonRelease(BUTTON_LEFT);
            // Display high scores when the left button is pressed
            displayManager.clearScreen();
            displayManager.drawScreen();
            highScoreManager.displayHighScores(tft);
            displayManager.displayNavigation("     ", "Back");
            inputHandler.waitForButtonClick(BUTTON_RIGHT); // Wait for a button click to return to start screen
            displayManager.clearScreen();
            displayManager.drawScreen();
            displayManager.displayStartScreen();
            displayManager.displayNavigation("Highscores", "Start");
        } else if (digitalRead(BUTTON_RIGHT) == LOW) {
            inputHandler.waitForButtonRelease(BUTTON_RIGHT);
            // Start the game when the right button is pressed
            break;
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
