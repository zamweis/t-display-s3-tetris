#include "DisplayManager.h"
#include "Game/ShapeFactory.h"

DisplayManager::DisplayManager(TFT_eSPI& tft) : tft(tft) {
    backgroundColor = tft.color565(30, 30, 30); // Set default background color
}

void DisplayManager::drawScreen() {
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

void DisplayManager::resetNameEntry() {
    // Set the text size and color explicitly for name entry
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    
    // Reset character entry variables
    currentCharIndex = 0;
    for (int i = 0; i < MAX_NAME_LENGTH; ++i) {
        playerName[i] = ' ';
    }
    currentChar = 'A';
    
    // Optionally, redraw the screen (make sure this doesn't interfere with text rendering)
    drawScreen();  // If this is not necessary, try commenting it out
    
    // Explicitly set text color again if needed
    tft.setTextColor(TFT_WHITE);
    
    // Redraw the header and name entry
    drawHeader();
    drawName();
}

void DisplayManager::drawGrid() {
     tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, backgroundColor);
    tft.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, TFT_BLACK);
    for (int col = 0; col <= SCREEN_WIDTH / BOX_SIZE; ++col) {
        tft.drawLine(col * BOX_SIZE, 0, col * BOX_SIZE, SCREEN_HEIGHT, TFT_BLACK);
    }
    for (int row = 0; row <= SCREEN_HEIGHT / BOX_SIZE; ++row) {
        tft.drawLine(0, row * BOX_SIZE, SCREEN_WIDTH, row * BOX_SIZE, TFT_BLACK);
    }
}

int DisplayManager::getCenteredX(const char* text) const {
    int textWidth = tft.textWidth(text);
    return (SCREEN_WIDTH - textWidth) / 2;
}

void DisplayManager::drawCenteredText(bool isGameOver, int score) {
     if (isGameOver) {
        // Display "Game Over"
        tft.setTextColor(TFT_RED);
        tft.setTextSize(2);
        const char* gameOverText = "GAME OVER";
        int gameOverTextX = getCenteredX(gameOverText);
        tft.setCursor(gameOverTextX, 50);
        tft.println(gameOverText);

        // Center and display "Score:"
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);
        const char* scoreTitle = "Score:";
        int scoreTitleX = getCenteredX(scoreTitle);
        tft.setCursor(scoreTitleX, 140);
        tft.println(scoreTitle);

        // Center and display the score value
        String scoreStr = String(score); // Convert score to string for width calculation
        int scoreX = getCenteredX(scoreStr.c_str());
        tft.setCursor(scoreX, 160);
        tft.printf("%d\n", score);

        // Center and display "Press" and "a button"
        const char* pressText = "Press";
        int pressTextX = getCenteredX(pressText);
        tft.setCursor(pressTextX, 200);
        tft.println(pressText);

        const char* buttonText = "a button";
        int buttonTextX = getCenteredX(buttonText);
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
        Shape* tempShape = ShapeFactory::createRandomShape();
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
        int pressTextX = getCenteredX(pressText);
        tft.setCursor(pressTextX, 200);
        tft.println(pressText);

        const char* rotateText = "Hold to rotate";
        int rotateTextX = getCenteredX(rotateText);
        tft.setCursor(rotateTextX, 220);
        tft.println(rotateText);
    }
}

int DisplayManager::getMoveDownSpeed(int level) {
    const int levelSpeeds[] = { 500, 450, 400, 350, 300, 250, 200, 190, 180, 170, 160, 150, 140, 130, 120, 110, 100, 90, 80, 60, 50 };
    return (level < sizeof(levelSpeeds) / sizeof(levelSpeeds[0])) ? levelSpeeds[level - 1] : levelSpeeds[sizeof(levelSpeeds) / sizeof(levelSpeeds[0]) - 1];
}

uint16_t DisplayManager::getBackgroundColor() const {
    return backgroundColor;
}

void DisplayManager::clearScreen() {
    tft.fillScreen(TFT_BLACK);
}

bool DisplayManager::promptPlayerForName() {
    bool nameConfirmed = false;
    bool blinkState = true;
    unsigned long lastBlinkTime = 0;
    const unsigned long blinkInterval = 500;

    tft.setTextSize(2);
    drawHeader(); // Draw the "New Highscore" header initially

    int nameEntryY = 120;
    drawName(); // Initial rendering of the name

    while (!nameConfirmed) {
        // Handle blinking
        if (millis() - lastBlinkTime >= blinkInterval) {
            blinkState = !blinkState;
            lastBlinkTime = millis();
            drawCurrentChar(blinkState, nameEntryY);
        }

        // Handle button input for cycling characters
        if (digitalRead(BUTTON_LEFT) == LOW) {
            waitForButtonClick(BUTTON_LEFT);
            cycleCharacter();
            drawCurrentChar(true, nameEntryY); 
            delay(200); // Debounce
        }

        // Handle button input for confirming character
        if (digitalRead(BUTTON_RIGHT) == LOW) {
            waitForButtonClick(BUTTON_RIGHT);
            confirmCurrentCharacter();
            drawConfirmedChar(nameEntryY);
            currentCharIndex++;
            if (currentCharIndex >= MAX_NAME_LENGTH) {
                nameConfirmed = promptNameConfirmation();
            } else {
                currentChar = 'A';
                delay(300);
            }
        }
    }
    return nameConfirmed;
}

void DisplayManager::waitForButtonClick(int buttonPin) {
    // Wait for the button to be pressed
    while (digitalRead(buttonPin) == HIGH) {
        // Do nothing, just wait for the button press
    }
    
    // Wait for the button to be released
    while (digitalRead(buttonPin) == LOW) {
        // Do nothing, just wait for the button release
    }
}

const char* DisplayManager::getPlayerName() const {
    static char playerName[] = "AAA"; // Replace with your logic
    return playerName;
}

void DisplayManager::drawHeader() {
    tft.setTextColor(TFT_WHITE);
    const char* newText = "New";
    int newTextWidth = tft.textWidth(newText);
    int centeredNewX = (SCREEN_WIDTH - newTextWidth) / 2;
    tft.setCursor(centeredNewX, 30);
    tft.setTextColor(TFT_WHITE);
    tft.println(newText);

    const char* highscoreText = "Highscore";
    int highscoreTextWidth = tft.textWidth(highscoreText);
    int centeredHighscoreX = (SCREEN_WIDTH - highscoreTextWidth) / 2;
    tft.setCursor(centeredHighscoreX, 50);
    tft.println(highscoreText);
}

void DisplayManager::cycleCharacter() {
    if (currentChar == ' ') {
        currentChar = 'A';
    } else {
        currentChar++;
        if (currentChar > 'Z') {
            currentChar = ' ';
        }
    }
    playerName[currentCharIndex] = currentChar;
}

void DisplayManager::drawConfirmedChar(int nameEntryY) {
    int x = 29 + currentCharIndex * 20;
    tft.fillRect(x, nameEntryY, 12, 20, TFT_BLACK);
    tft.setCursor(x, nameEntryY);
    tft.setTextColor(TFT_WHITE);
    tft.print(playerName[currentCharIndex]);
    tft.drawLine(x, nameEntryY + 20, x + 12, nameEntryY + 20, TFT_BLACK);
}

bool DisplayManager::promptNameConfirmation() {
    bool confirmChoice = false;
    while (!confirmChoice) {
        // Clear and display options
        tft.fillRect(0, SCREEN_HEIGHT - 40, SCREEN_WIDTH, 40, TFT_BLACK);
        int backX = 20; 
        int backY = SCREEN_HEIGHT - 30; 
        tft.setTextSize(1);
        tft.setCursor(backX, backY);
        tft.print("Back");
        int enterX = SCREEN_WIDTH - tft.textWidth("Enter") - 20; 
        int enterY = SCREEN_HEIGHT - 30; 
        tft.setCursor(enterX, enterY);
        tft.print("Enter");

        // Wait for button input
        if (digitalRead(BUTTON_RIGHT) == LOW) {
            waitForButtonClick(BUTTON_RIGHT);
            delay(100); // Debounce delay
            return true;
        } else if (digitalRead(BUTTON_LEFT) == LOW) {
            waitForButtonClick(BUTTON_LEFT);
            resetNameEntry();
            delay(100); // Debounce delay
            return false;
        }
        delay(300); // Debounce delay to avoid rapid checks
    }
    return false;
}

void DisplayManager::drawCurrentChar(bool highlight, int nameEntryY) {
    int x = 29 + currentCharIndex * 20;
    tft.fillRect(x, nameEntryY, 12, 20, TFT_BLACK);
    tft.setCursor(x, nameEntryY);
    tft.setTextColor(highlight ? TFT_YELLOW : TFT_WHITE);
    tft.print(playerName[currentCharIndex]);
    tft.drawLine(x, nameEntryY + 20, x + 12, nameEntryY + 20, highlight ? TFT_YELLOW : TFT_BLACK);
}

void DisplayManager::confirmCurrentCharacter() {
    playerName[currentCharIndex] = currentChar;
}

void DisplayManager::drawName() {
    tft.setTextColor(TFT_WHITE);
    int nameEntryY = 120;
    for (int i = 0; i < MAX_NAME_LENGTH; ++i) {
        int x = 29 + i * 20;
        tft.setCursor(x, nameEntryY);
        tft.print(playerName[i]);
    }
}