#include <Arduino.h>
#include "TFT_eSPI.h"
#include "Shapes/ShapeI.h"
#include "Shapes/ShapeJ.h"
#include "Shapes/ShapeL.h"
#include "Shapes/ShapeO.h"
#include "Shapes/ShapeS.h"
#include "Shapes/ShapeT.h"
#include "Shapes/ShapeZ.h"

#define SCREEN_WIDTH 170
#define SCREEN_HEIGHT 320
#define BOX_SIZE 17

#define BACKLIGHT_PIN 38 // Use the TFT_BL pin specified in your configuration
#define BUTTON_LEFT 0    // BOOT button pin (GPIO0)
#define BUTTON_RIGHT 14  // IO14 button pin

TFT_eSPI tft = TFT_eSPI();  // Initialize TFT display
Shape* shape = nullptr;
BlockMap blockMap;

uint16_t backgroundColor;

// Game variables
unsigned long lastMoveDownTime = 0;
unsigned long lastRotateLeftTime = 0;
unsigned long lastRotateRightTime = 0;
const unsigned long initialMoveDelay = 120;
const unsigned long rotationHoldThreshold = 300;
const unsigned long rotationDebounceInterval = 250;

int score = 0;
int level = 1;
int linesCleared = 0;

enum ButtonState { IDLE, PRESSED, SHORT_PRESS, LONG_PRESS };
ButtonState leftButtonState = IDLE;
ButtonState rightButtonState = IDLE;
unsigned long buttonPressStart = 0;

// Level speed table (values in milliseconds)
const int levelSpeeds[] = { 300, 250, 200, 150, 100, 80, 70, 60, 50, 45, 40, 35, 30, 25, 20, 15 };

int getMoveDownSpeed() {
    return (level < sizeof(levelSpeeds) / sizeof(levelSpeeds[0])) ? levelSpeeds[level - 1] : levelSpeeds[sizeof(levelSpeeds) / sizeof(levelSpeeds[0]) - 1];
}

Shape* createRandomShape() {
    int index = rand() % 7;
    switch (index) {
        case 0: return new ShapeI();
        case 1: return new ShapeJ();
        case 2: return new ShapeL();
        case 3: return new ShapeO();
        case 4: return new ShapeS();
        case 5: return new ShapeT();
        case 6: return new ShapeZ();
        default: return nullptr;
    }
}

void drawGrid() {
    tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, backgroundColor);
    tft.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, TFT_BLACK);
    for (int col = 0; col <= SCREEN_WIDTH / BOX_SIZE; ++col) {
        tft.drawLine(col * BOX_SIZE, 0, col * BOX_SIZE, SCREEN_HEIGHT, TFT_BLACK);
    }
    for (int row = 0; row <= SCREEN_HEIGHT / BOX_SIZE; ++row) {
        tft.drawLine(0, row * BOX_SIZE, SCREEN_WIDTH, row * BOX_SIZE, TFT_BLACK);
    }
}

void updateScoreAndLevel(int clearedLines) {
    const int pointsPerLine[4] = {40, 100, 300, 1200};
    if (clearedLines > 0 && clearedLines <= 4) {
        score += pointsPerLine[clearedLines - 1] * (level + 1);
        linesCleared += clearedLines;
        level = 1 + linesCleared / 10;
        Serial.printf("Score: %d, Level: %d\n", score, level);
    }
}

void resetGame() {
    score = 0;
    level = 1;
    linesCleared = 0;
    delete shape;
    shape = nullptr;
    blockMap = BlockMap();
    drawGrid();
    shape = createRandomShape();
    if (shape) {
        shape->moveToLowestBlockkAtMinusOne();
        shape->drawShape(tft, BOX_SIZE);
    }
}

// Function to calculate the x-coordinate to center text on the screen
int getCenteredX(const char* text, TFT_eSPI &display) {
    int textWidth = display.textWidth(text);
    return (SCREEN_WIDTH - textWidth) / 2;
}

// Usage example for centering text in your main code
void drawCenteredText(bool isGameOver, int score) {
    if (isGameOver) {
        // Display "Game Over"
        tft.setTextColor(TFT_RED);
        tft.setTextSize(2);
        const char* gameOverText = "GAME OVER";
        int gameOverTextX = getCenteredX(gameOverText, tft);
        tft.setCursor(gameOverTextX, 50);
        tft.println(gameOverText);

        // Center and display "Score:"
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);
        const char* scoreTitle = "Score:";
        int scoreTitleX = getCenteredX(scoreTitle, tft);
        tft.setCursor(scoreTitleX, 140);
        tft.println(scoreTitle);

        // Center and display the score value
        String scoreStr = String(score); // Convert score to string for width calculation
        int scoreX = getCenteredX(scoreStr.c_str(), tft);
        tft.setCursor(scoreX, 160);
        tft.printf("%d\n", score);

        // Center and display "Press" and "a button"
        const char* pressText = "Press";
        int pressTextX = getCenteredX(pressText, tft);
        tft.setCursor(pressTextX, 200);
        tft.println(pressText);

        const char* buttonText = "a button";
        int buttonTextX = getCenteredX(buttonText, tft);
        tft.setCursor(buttonTextX, 220);
        tft.println(buttonText);
    } else {
        // Display "Welcome"
        tft.setTextColor(TFT_GREEN);
        tft.setTextSize(3);
        const char* tetrisText = "Tetris";
        const uint16_t colors[] = {TFT_RED, TFT_BLUE, TFT_GREEN, TFT_YELLOW, TFT_ORANGE, TFT_CYAN, TFT_PURPLE};
        int numColors = sizeof(colors) / sizeof(colors[0]);
        int textLength = strlen(tetrisText);

        // Calculate starting X position for centering
        int totalWidth = 0;
        for (int i = 0; i < textLength; i++) {
            totalWidth += tft.textWidth(String(tetrisText[i]).c_str());
        }
        int startX = (SCREEN_WIDTH - totalWidth) / 2;

        // Display each letter with a different color
        int currentX = startX;
        for (int i = 0; i < textLength; i++) {
            tft.setTextColor(colors[i % numColors]);  // Cycle through colors
            tft.setCursor(currentX, 50);
            tft.print(tetrisText[i]);
            currentX += tft.textWidth(String(tetrisText[i]).c_str());  // Move X position for next character
        }

        // Center a random shape under "WELCOME"
        Shape* tempShape = createRandomShape();
        if (tempShape) {
            int shapeWidth = (tempShape->getWidth() - 1)* BOX_SIZE; // Ensure getWidth() returns correct number of blocks
            int shapeX = (SCREEN_WIDTH - shapeWidth) / 2; // Calculate x-coordinate to center shape
            tempShape->setPosition(shapeX / BOX_SIZE, 7); // Adjust y-position below "WELCOME"
            tempShape->drawShape(tft, BOX_SIZE);
            delete tempShape; // Clean up the temporary shape
        }

        // Display controls
        tft.setTextSize(1);
        tft.setTextColor(TFT_WHITE);
        const char* pressText = "Press left or right";
        int pressTextX = getCenteredX(pressText, tft);
        tft.setCursor(pressTextX, 200);
        tft.println(pressText);

        const char* rotateText = "Hold to rotate";
        int rotateTextX = getCenteredX(rotateText, tft);
        tft.setCursor(rotateTextX, 220);
        tft.println(rotateText);
    }
}

void drawScreen(bool isGameOver) {
    tft.fillScreen(TFT_BLACK);

    int blockSize = BOX_SIZE;
    uint16_t colors[] = {TFT_GREEN, TFT_YELLOW, TFT_RED, TFT_BLUE};
    int numColors = sizeof(colors) / sizeof(colors[0]);

    for (int i = 0; i < SCREEN_WIDTH; i += blockSize) {
        Block borderBlock(i / blockSize, 0, colors[i / blockSize % numColors]);
        borderBlock.draw(tft, blockSize);
        borderBlock = Block(i / blockSize, SCREEN_HEIGHT / blockSize, colors[i / blockSize % numColors]);
        borderBlock.draw(tft, blockSize);
    }
    for (int j = 0; j < SCREEN_HEIGHT; j += blockSize) {
        Block borderBlock(0, j / blockSize, colors[j / blockSize % numColors]);
        borderBlock.draw(tft, blockSize);
        borderBlock = Block((SCREEN_WIDTH - blockSize) / blockSize, j / blockSize, colors[j / blockSize % numColors]);
        borderBlock.draw(tft, blockSize);
    }

    drawCenteredText(isGameOver, score);
}

void setup() {
    Serial.begin(115200);
    tft.init();
    pinMode(BACKLIGHT_PIN, OUTPUT);
    analogWrite(BACKLIGHT_PIN, 100);
    tft.setRotation(0);
    backgroundColor = tft.color565(30, 30, 30);
    drawScreen(false);
    while (digitalRead(BUTTON_LEFT) == HIGH && digitalRead(BUTTON_RIGHT) == HIGH) {}
    resetGame();
}

void handleButtonState(ButtonState &state, int buttonPin, unsigned long currentTime, void (Shape::*moveFunc)(BlockMap &), void (Shape::*rotateFunc)(BlockMap &)) {
    switch (state) {
        case IDLE:
            if (digitalRead(buttonPin) == LOW) {
                state = PRESSED;
                buttonPressStart = currentTime;
            }
            break;
        case PRESSED:
            if (digitalRead(buttonPin) == HIGH) {
                state = (currentTime - buttonPressStart < rotationHoldThreshold) ? SHORT_PRESS : IDLE;
            } else if (currentTime - buttonPressStart >= rotationHoldThreshold) {
                state = LONG_PRESS;
            }
            break;
        case SHORT_PRESS:
            if (shape) {
                shape->eraseShape(tft, BOX_SIZE, backgroundColor);
                (shape->*moveFunc)(blockMap);
                shape->drawShape(tft, BOX_SIZE);
            }
            state = IDLE;
            break;
        case LONG_PRESS:
            if (digitalRead(buttonPin) == HIGH) {
                state = IDLE;
            } else {
                static unsigned long lastRotateTime = 0;
                if (currentTime - lastRotateTime >= rotationDebounceInterval) {
                    if (shape) {
                        shape->eraseShape(tft, BOX_SIZE, backgroundColor);
                        (shape->*rotateFunc)(blockMap);
                        shape->drawShape(tft, BOX_SIZE);
                        lastRotateTime = currentTime;
                    }
                }
            }
            break;
    }
}

void loop() {
    unsigned long currentTime = millis();
    if (blockMap.checkGameOver()) {
        drawScreen(true);
        while (digitalRead(BUTTON_LEFT) == HIGH && digitalRead(BUTTON_RIGHT) == HIGH) {}
        resetGame();
        return;
    }
    handleButtonState(leftButtonState, BUTTON_LEFT, currentTime, &Shape::moveLeft, &Shape::rotateAntiClockwise);
    handleButtonState(rightButtonState, BUTTON_RIGHT, currentTime, &Shape::moveRight, &Shape::rotateClockwise);

    if (shape) {
        if (currentTime - lastMoveDownTime >= getMoveDownSpeed()) {
            lastMoveDownTime = currentTime;
            if (shape->isMovableDownWards(blockMap)) {
                shape->eraseShape(tft, BOX_SIZE, backgroundColor);
                shape->moveDown(blockMap);
                shape->drawShape(tft, BOX_SIZE);
            } else {
                blockMap.addBlocks(shape->getBlockList(), 4);
                int clearedLines = blockMap.clearAndMoveAllFullLines(tft, BOX_SIZE, backgroundColor);
                if (clearedLines > 0) updateScoreAndLevel(clearedLines);
                shape->eraseShape(tft, BOX_SIZE, backgroundColor);
                blockMap.drawAllBlocks(tft, BOX_SIZE);
                delete shape;
                shape = nullptr;
            }
        }
    } else {
        shape = createRandomShape();
        if (shape) {
            shape->moveToLowestBlockkAtMinusOne();
            shape->drawShape(tft, BOX_SIZE);
        }
    }
    blockMap.drawAllBlocks(tft, BOX_SIZE);
}
