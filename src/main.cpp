#include <Arduino.h>
#include "TFT_eSPI.h"
#include "Shapes/ShapeI.h"
#include "Shapes/ShapeJ.h"
#include "Shapes/ShapeL.h"
#include "Shapes/ShapeO.h"
#include "Shapes/ShapeS.h"
#include "Shapes/ShapeT.h"
#include "Shapes/ShapeZ.h"
#include <Preferences.h>

#define SCREEN_WIDTH 170
#define SCREEN_HEIGHT 320
#define BOX_SIZE 17

#define BACKLIGHT_PIN 38 // Use the TFT_BL pin specified in your configuration
#define BUTTON_LEFT 0    // BOOT button pin (GPIO0)
#define BUTTON_RIGHT 14  // IO14 button pin

TFT_eSPI tft = TFT_eSPI();  // Initialize TFT display
Shape* shape = nullptr;
BlockMap blockMap;
Preferences preferences;
// Highscore variables
struct HighScore {
    char name[7]; // 3 characters for name + null terminator
    int score;
};

const int maxHighScores = 15;
HighScore highScores[maxHighScores];

uint16_t backgroundColor;

// Game variables
unsigned long lastMoveDownTime = 0;
unsigned long lastRotateLeftTime = 0;
unsigned long lastRotateRightTime = 0;
const unsigned long initialMoveDelay = 120;
const unsigned long rotationHoldThreshold = 300;
const unsigned long rotationDebounceInterval = 250;

const int MAX_NAME_LENGTH = 6;
char playerName[MAX_NAME_LENGTH] = {'A', 'A', 'A', 'A', 'A', 'A'};
int currentCharIndex = 0;
char currentChar = 'A';

int score = 6000;
int level = 9;
int linesCleared = 60;

enum ButtonState { IDLE, PRESSED, SHORT_PRESS, LONG_PRESS };
ButtonState leftButtonState = IDLE;
ButtonState rightButtonState = IDLE;
unsigned long buttonPressStart = 0;

// Level speed table (values in milliseconds)
const int levelSpeeds[] = { 100 };

int getMoveDownSpeed() {
    return (level < sizeof(levelSpeeds) / sizeof(levelSpeeds[0])) ? levelSpeeds[level - 1] : levelSpeeds[sizeof(levelSpeeds) / sizeof(levelSpeeds[0]) - 1];
}

Shape* createRandomShape() {
    int index = random(0, 7);
    index = 3;
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

void waitForButtonRelease(int buttonPin) {
    while (digitalRead(buttonPin) == LOW) {
        // Do nothing and wait for the button to be released
    }
}

void waitForButtonClick(int buttonPin) {
    // Wait for the button to be pressed
    while (digitalRead(buttonPin) == HIGH) {
        // Do nothing, just wait for the button press
    }
    
    // Wait for the button to be released
    while (digitalRead(buttonPin) == LOW) {
        // Do nothing, just wait for the button release
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

void loadHighScores(HighScore scores[], int maxScores) {
    preferences.begin("highScores", true); // Open in read-only mode
    for (int i = 0; i < maxScores; ++i) {
        String keyName = "name" + String(i);
        String keyScore = "score" + String(i);
        scores[i].score = preferences.getInt(keyScore.c_str(), 0);
        String storedName = preferences.getString(keyName.c_str(), "---");
        storedName.toCharArray(scores[i].name, sizeof(scores[i].name));
    }
    preferences.end();
}

void saveHighScores(HighScore scores[], int maxScores) {
    preferences.begin("highScores", false); // Open in write mode
    for (int i = 0; i < maxScores; ++i) {
        String keyName = "name" + String(i);
        String keyScore = "score" + String(i);
        preferences.putInt(keyScore.c_str(), scores[i].score);
        preferences.putString(keyName.c_str(), String(scores[i].name));
    }
    preferences.end();
}

void updateHighScores(int newScore, const char* playerName) {
    loadHighScores(highScores, maxHighScores);

    // Insert the new score if it's high enough
    for (int i = 0; i < maxHighScores; ++i) {
        if (newScore > highScores[i].score) {
            // Shift lower scores down
            for (int j = maxHighScores - 1; j > i; --j) {
                highScores[j] = highScores[j - 1];
            }
            // Insert the new score
            strncpy(highScores[i].name, playerName, sizeof(highScores[i].name) - 1);
            highScores[i].name[sizeof(highScores[i].name) - 1] = '\0'; // Ensure null-terminated
            highScores[i].score = newScore;
            break;
        }
    }

    // Save updated high scores
    saveHighScores(highScores, maxHighScores);
}

void displayHighScores() {
    tft.setTextSize(2); // Smaller text size to fit better within the screen
    tft.setTextColor(TFT_WHITE);

    const char* titleText = "High Scores";
    int titleWidth = tft.textWidth(titleText);
    int titleX = (SCREEN_WIDTH - titleWidth) / 2; // Center the title
    tft.setCursor(titleX, 35); // Position title near the top
    tft.println(titleText);
    tft.setTextSize(1);

    // Determine the maximum text width for consistent alignment
    char sampleEntry[32];
    snprintf(sampleEntry, sizeof(sampleEntry), "%d. %s - %d", maxHighScores, "ABCDEF", 99999); // Sample longest entry
    int maxTextWidth = tft.textWidth(sampleEntry);
    int alignedLeftX = (SCREEN_WIDTH - maxTextWidth) / 2; // Calculate alignment point for consistency

    // Display each high score entry
    for (int i = 0; i < maxHighScores; ++i) {
        // Format the text to be displayed
        char scoreEntry[32];
        snprintf(scoreEntry, sizeof(scoreEntry), "%d. %s - %d", i + 1, highScores[i].name, highScores[i].score);

        // Display the text, aligned to the calculated left position
        int y = 65 + i * 15; // Adjust vertical spacing as needed
        tft.setCursor(alignedLeftX, y);
        tft.println(scoreEntry);
    }
}

bool promptPlayerForName() {
    bool nameConfirmed = false;
    bool blinkState = true;
    unsigned long lastBlinkTime = 0;
    const unsigned long blinkInterval = 500; // 500ms blink interval

    // Initial setup for drawing
    tft.setTextSize(2);

    // Display "New" on the first line
    const char* newText = "New";
    int newTextWidth = tft.textWidth(newText);
    int centeredNewX = (SCREEN_WIDTH - newTextWidth) / 2; // Center the text horizontally
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(centeredNewX, 30); // Set position below border
    tft.println(newText);

    // Display "Highscore" on the next line
    const char* highscoreText = "Highscore";
    int highscoreTextWidth = tft.textWidth(highscoreText);
    int centeredHighscoreX = (SCREEN_WIDTH - highscoreTextWidth) / 2; // Center the text horizontally
    tft.setCursor(centeredHighscoreX, 50); // Adjust vertical position as needed
    tft.println(highscoreText);

    // Set vertical position for name entry below title
    int nameEntryY = 120; // Adjusted for centered vertical positioning

    // Display initial name characters
    for (int i = 0; i < MAX_NAME_LENGTH; ++i) {
        int x = 29 + i * 20; // Horizontal spacing between characters, shifted by an additional 2 pixels
        tft.setCursor(x, nameEntryY);
        tft.print(playerName[i]);
    }

    while (!nameConfirmed) {
        // Handle blinking logic
        if (millis() - lastBlinkTime >= blinkInterval) {
            blinkState = !blinkState;
            lastBlinkTime = millis();

            // Reset character display
            int x = 29 + currentCharIndex * 20; // Apply the additional 2-pixel shift here as well
            tft.fillRect(x, nameEntryY, 12, 20, TFT_BLACK); // Clear character area
            tft.setCursor(x, nameEntryY);
            tft.setTextColor(blinkState ? TFT_YELLOW : TFT_WHITE); // Toggle color
            tft.print(playerName[currentCharIndex]);
            // Handle underline removal
            tft.drawLine(x, nameEntryY + 20, x + 12, nameEntryY + 20, blinkState ? TFT_YELLOW : TFT_BLACK);
        }

        // Handle button input
        if (digitalRead(BUTTON_LEFT) == LOW) {
            waitForButtonClick(BUTTON_LEFT);

            // Cycle through characters (A-Z, space)
            if (currentChar == ' ') {
                currentChar = 'A';
            } else {
                currentChar++;
                if (currentChar > 'Z') {
                    currentChar = ' '; // Allow space character
                }
            }
            playerName[currentCharIndex] = currentChar;

            // Update character display
            int x = 29 + currentCharIndex * 20; // Apply the additional 2-pixel shift here too
            tft.fillRect(x, nameEntryY, 12, 20, TFT_BLACK); // Clear character area
            tft.setCursor(x, nameEntryY);
            tft.setTextColor(TFT_YELLOW); // Highlight current character
            tft.print(playerName[currentCharIndex]);
            tft.drawLine(x, nameEntryY + 20, x + 12, nameEntryY + 20, TFT_BLACK); // Remove underline
            delay(200); // Debounce delay
        }

        if (digitalRead(BUTTON_RIGHT) == LOW) {
            waitForButtonClick(BUTTON_RIGHT);

            // Confirm current character and move to next
            playerName[currentCharIndex] = currentChar;
            int x = 29 + currentCharIndex * 20; // Apply the additional 2-pixel shift here as well
            tft.fillRect(x, nameEntryY, 12, 20, TFT_BLACK); // Clear character area
            tft.setCursor(x, nameEntryY);
            tft.setTextColor(TFT_WHITE); // Confirmed character color
            tft.print(playerName[currentCharIndex]);
            tft.drawLine(x, nameEntryY + 20, x + 12, nameEntryY + 20, TFT_BLACK); // Clear underline

            currentCharIndex++;

            if (currentCharIndex >= MAX_NAME_LENGTH) {
                // Prompt to confirm or redefine name
                bool confirmChoice = false;
                while (!confirmChoice) {
                    tft.fillRect(0, 180, SCREEN_WIDTH, 80, TFT_BLACK); // Clear lower screen section
                    
                    int backX = 20; // Small margin from the left border
                    int backY = SCREEN_HEIGHT - 30; // Near the bottom of the screen, adjusting for text size

                    tft.setTextSize(1);
                    tft.setCursor(backX, backY);
                    tft.print("Back");

                    // Set the cursor for "Enter" at the lower right corner
                    int enterX = SCREEN_WIDTH - tft.textWidth("Enter") - 20; // Small margin from the right border
                    int enterY = SCREEN_HEIGHT - 30; // Same vertical position as "Back"
                    tft.setCursor(enterX, enterY);
                    tft.print("Enter");

                    if (digitalRead(BUTTON_RIGHT) == LOW) {
                        waitForButtonClick(BUTTON_RIGHT);
                        nameConfirmed = true;
                        confirmChoice = true;
                    } else if (digitalRead(BUTTON_LEFT) == LOW) {
                        waitForButtonClick(BUTTON_LEFT);

                        // Reset for name redefinition
                        currentCharIndex = 0;
                        for (int i = 0; i < MAX_NAME_LENGTH; ++i) {
                            playerName[i] = ' ';
                        }
                        currentChar = 'A'; // Reset character
                        confirmChoice = true;

                        // Redraw initial state for re-entry
                        tft.fillRect(0, 40, SCREEN_WIDTH, SCREEN_HEIGHT - 40, TFT_BLACK);
                        tft.setTextColor(TFT_WHITE);
                        tft.setCursor(centeredNewX, 20);
                        tft.println(newText);
                        tft.setCursor(centeredHighscoreX, 40);
                        tft.println(highscoreText);

                        for (int i = 0; i < MAX_NAME_LENGTH; ++i) {
                            int x = 29 + i * 20; // Apply the additional 2-pixel shift here as well
                            tft.setCursor(x, nameEntryY);
                            tft.print(playerName[i]);
                        }
                    }
                    delay(300); // Debounce delay
                }

                return nameConfirmed;
            } else {
                // Move to next character
                currentChar = 'A';
                delay(300); // Debounce delay
            }
        }
    }
    return nameConfirmed;
}

bool isHighScore(int newScore) {
    loadHighScores(highScores, maxHighScores); // Load current high scores
    for (int i = 0; i < maxHighScores; ++i) {
        if (newScore > highScores[i].score) {
            return true;
        }
    }
    return false;
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

void drawScreen() {
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
}

void handleGameOver() {
    drawScreen();
    drawCenteredText(true, score); // Display "Game Over" and score

    // Wait for button press to return to the start screen
    waitForButtonClick(BUTTON_RIGHT);

    if (isHighScore(score)) {
        tft.fillScreen(TFT_BLACK);
        drawScreen();

        // Prompt player for name entry
        if (promptPlayerForName()) {
            updateHighScores(score, playerName); // Update the high scores
        }
    } 
    drawScreen();
    displayHighScores();
    

    // Wait for button press to return to the start screen
    waitForButtonClick(BUTTON_RIGHT);
    tft.fillScreen(TFT_BLACK);
    drawScreen();
    drawCenteredText(false, score);
    waitForButtonClick(BUTTON_RIGHT);

    resetGame(); // Reset game state
}

void setup() {
    randomSeed(analogRead(0));

    Serial.begin(115200);
    tft.init();
    pinMode(BACKLIGHT_PIN, OUTPUT);
    analogWrite(BACKLIGHT_PIN, 100);
    tft.setRotation(0);
    backgroundColor = tft.color565(30, 30, 30);
    drawScreen();
    drawCenteredText(false, score);
    waitForButtonClick(BUTTON_RIGHT);
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

void updateScoreAndLevel(int clearedLines) {
    const int pointsPerLine[4] = {40, 100, 300, 1200};
    if (clearedLines > 0 && clearedLines <= 4) {
        score += pointsPerLine[clearedLines - 1] * (level + 1);
        linesCleared += clearedLines;
        level = 1 + linesCleared / 10;
        Serial.printf("Score: %d, Level: %d\n", score, level);
    }
}


void loop() {
    unsigned long currentTime = millis();
    if (blockMap.checkGameOver()) {
        handleGameOver(); // Handle the game-over logic
        return;
    }

    // Handle button states and shape movements
    handleButtonState(leftButtonState, BUTTON_LEFT, currentTime, &Shape::moveLeft, &Shape::rotateAntiClockwise);
    handleButtonState(rightButtonState, BUTTON_RIGHT, currentTime, &Shape::moveRight, &Shape::rotateClockwise);

    // Shape movement logic
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

