#include "Game/Game.h"
#include "Config.h"
#include "TetrisAI/TetrisAI.h"
#include "ShapeFactory.h"

TetrisAI::Move currentMove;
bool isGravityActive = false;
bool directionChosen = false;

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
        Serial.println("Game Over detected.");
        handleGameOver();
        return;
    }

    handleShapeMovement(currentTime);

    if (!shape) {
        createNewShape();
    } else {
        static unsigned long lastAIStepTime = 0;
        unsigned long aiStepInterval = 200; // 100ms interval for AI step

        if (currentTime - lastAIStepTime >= aiStepInterval) {
            executeAIStep();
            lastAIStepTime = currentTime; // Update the last AI step time
        }

        if (!shape) {
            createNewShape();
        }
        updateShapePosition(currentTime);
    }

    blockMap.drawAllBlocks(tft);
}

void Game::handleShapeMovement(unsigned long currentTime) {
    handleButtonState(leftButtonState, BUTTON_LEFT, currentTime, &Shape::moveLeft, &Shape::rotateAntiClockwise);
    handleButtonState(rightButtonState, BUTTON_RIGHT, currentTime, &Shape::moveRight, &Shape::rotateClockwise);
}

void Game::createNewShape() {
    shape = ShapeFactory::createRandomShape();
    if (shape) {
        if (!shape->canMoveToPosition(shape->getBlock(0).getX(), shape->getBlock(0).getY(), blockMap)) {
            delete shape;
            shape = nullptr;
            return;
        }

        shape->moveToLowestBlockkAtMinusOne();
        shape->drawShape(tft);
    } else {
        Serial.println("Error: Shape could not be created.");
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
        return false; // Wait until gravity finishes
    }

    // Step 1: Calculate the best move if not already done
    if (currentMove.score == std::numeric_limits<int>::min()) {
        Serial.println("AI: Starting best move calculation.");
        if (!calculateBestMove()) {
            Serial.println("AI: No valid move found. Skipping turn.");
            return false; // No valid move found
        }
        Serial.printf("AI: Best move calculated: X=%d, Rotation=%d.\n", currentMove.x, currentMove.rotation);
    }

    // Step 2: Rotate to the desired position
    Serial.printf("AI: Aligning rotation. Current=%d, Target=%d.\n", shape->getRotatePosition(), currentMove.rotation);
    if (!alignShapeRotation()) {
        Serial.println("AI: Waiting for rotation alignment.");
        return true; // Wait for the next loop iteration to continue
    }

    // Step 3: Validate rotation before moving horizontally
    if (shape->getRotatePosition() != currentMove.rotation) {
        Serial.printf("AI: Rotation mismatch. Current=%d, Target=%d. Recalculating move.\n",
                      shape->getRotatePosition(), currentMove.rotation);
        currentMove.score = std::numeric_limits<int>::min(); // Force recalculation
        return false;
    }

    // Step 4: Move horizontally to the target column
    Serial.printf("AI: Preparing horizontal movement. CurrentX=%d, TargetX=%d.\n", shape->getBlock(0).getX(), currentMove.x);
    if (!moveShapeToTargetColumn()) {
        Serial.println("AI: Waiting for horizontal alignment.");
        return true; // Wait for the next loop iteration to continue
    }

    // Step 5: Drop the shape once it's aligned
    Serial.println("AI: Dropping the shape.");
    dropShape();

    return true;
}

bool Game::calculateBestMove() {
    currentMove = tetrisAI.findBestMove(blockMap, *shape);

    if (currentMove.score == std::numeric_limits<int>::min()) {
        Serial.println("AI could not find a valid move.");
        return false;
    }

    Serial.printf("AI move calculated: X=%d, Rotation=%d\n", currentMove.x, currentMove.rotation);
    return true;
}

bool Game::alignShapeRotation() {
    static bool rotateClockwise;  // Persist the chosen rotation direction
    int currentRotation = shape->getRotatePosition();
    int targetRotation = currentMove.rotation;

    if (shape->getRotatePosition() == targetRotation) {
        Serial.println("AI: Rotation alignment complete.");
        directionChosen = false;  // Reset for the next alignment
        return true;
    }
    
    // Determine the shortest rotation direction if not already chosen
    if (!directionChosen) {
        int clockwiseSteps = (targetRotation - currentRotation + 4) % 4;
        int antiClockwiseSteps = (currentRotation - targetRotation + 4) % 4;
        rotateClockwise = clockwiseSteps <= antiClockwiseSteps;
        directionChosen = true;  // Lock the chosen direction
        Serial.printf("AI: Chosen rotation direction: %s\n", rotateClockwise ? "Clockwise" : "Anti-clockwise");
    }

    // Erase the shape before attempting to rotate
    shape->eraseShape(tft, displayManager.getBackgroundColor());

    // Attempt to rotate in the chosen direction
    if (rotateClockwise) {
        if (shape->isRotatableClockwise(blockMap)) {
            shape->rotateClockwise(blockMap);
        } else {
            Serial.printf("AI: Cannot rotate clockwise. Switching to anti-clockwise.\n");
            rotateClockwise = false;  // Change direction
        }
    } else {
        if (shape->isRotatableAntiClockwise(blockMap)) {
            shape->rotateAntiClockwise(blockMap);
        } else {
            Serial.printf("AI: Cannot rotate anti-clockwise. Switching to clockwise.\n");
            rotateClockwise = true;  // Change direction
        }
    }

    // Draw the shape after rotation
    shape->drawShape(tft);

    // Debug the current rotation
    Serial.printf("AI: Rotated shape to position %d.\n", shape->getRotatePosition());

    // Check if alignment is complete
    if (shape->getRotatePosition() == targetRotation) {
        Serial.println("AI: Rotation alignment complete.");
        directionChosen = false;  // Reset for the next alignment
        return true;
    }

    return false;  // Alignment not yet complete
}

bool Game::moveShapeToTargetColumn() {
    if (shape->getRotatePosition() != currentMove.rotation) {
        Serial.println("AI: Shape rotation mismatch detected. Aborting horizontal movement.");
        return false;
    }

    int shapeX = shape->getBlock(0).getX(); // Main block X position

    if (shapeX != currentMove.x) {
        if (shapeX < currentMove.x && shape->isMovableToTheRight(blockMap)) {
            shape->eraseShape(tft, displayManager.getBackgroundColor());
            shape->moveRight(blockMap);
            shape->drawShape(tft);
            Serial.printf("AI: Moved shape right to X=%d.\n", shape->getBlock(0).getX());
        } else if (shapeX > currentMove.x && shape->isMovableToTheLeft(blockMap)) {
            shape->eraseShape(tft, displayManager.getBackgroundColor());
            shape->moveLeft(blockMap);
            shape->drawShape(tft);
            Serial.printf("AI: Moved shape left to X=%d.\n", shape->getBlock(0).getX());
        } else {
            Serial.printf("AI: Cannot move shape to X=%d. Aborting move.\n", currentMove.x);
            currentMove.score = std::numeric_limits<int>::min(); // Force recalculation
            return false; // Abort movement
        }
        return false; // Wait for the next loop iteration to continue
    }

    return true; // Target column is reached
}

void Game::dropShape() {
    shape->eraseShape(tft, displayManager.getBackgroundColor());
    shape->fallDown(blockMap);
    finalizeShapePlacement();
    shape->drawShape(tft);

    Serial.println("AI dropped the shape to finalize placement.");

    // Reset shape and AI move for the next turn
    delete shape;
    shape = nullptr;
    currentMove.score = std::numeric_limits<int>::min();
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
            finalizeShapePlacement();
            delete shape;
            shape = nullptr;
            currentMove.score = std::numeric_limits<int>::min();
        }

        isGravityActive = false;
    }
}

void Game::finalizeShapePlacement() {
    blockMap.addBlocks(shape->getBlockList(), Shape::NUM_BLOCKS);
    blockMap.printBlockMap();

    int clearedLines = blockMap.clearAndMoveAllFullLines(tft, displayManager.getBackgroundColor());
    if (clearedLines > 0) {
        Serial.printf("Lines cleared: %d\n", clearedLines);
        updateScoreAndLevel(clearedLines);
    } else {
        Serial.println("No lines cleared.");
    }
}
