#include "DisplayManager.h"
#include "Game/ShapeFactory.h"
#include "Input/InputHandler.h"

DisplayManager::DisplayManager(TFT_eSPI& tft)
    : tft(tft), inputHandler(BUTTON_LEFT, BUTTON_RIGHT) // Initialize inputHandler here
{
    backgroundColor = tft.color565(30, 30, 30); // Set default background color
    currentCharIndex = 0;
    currentChar = 'A';
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
    // Reset character entry variables
    currentCharIndex = 0;
    currentChar = 'A';
    for (int i = 0; i < MAX_NAME_LENGTH; ++i) {
        playerName[i] = 'A';
    }
    playerName[MAX_NAME_LENGTH] = '\0'; // Null-terminate for safety if needed
}

void DisplayManager::retryNameEntry() {
    // Set the text size and color explicitly for name entry
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    
    resetNameEntry();
    
    // Optionally, redraw the screen (make sure this doesn't interfere with text rendering)
    drawScreen();  // If this is not necessary, try commenting it out
    
    // Explicitly set text color again if needed
    tft.setTextColor(TFT_WHITE);
    
    // Redraw the header and name entry
    drawHeader();
    drawName();
    displayNavigation("Change", "Enter");
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

void DisplayManager::displayGameOverScreen(int score){
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

        displayNavigation("      ", "Next");
        tft.setTextSize(2);
}

void DisplayManager::displayStartScreen() {
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

    // Center a random shape under "Tetris"
    Shape* tempShape = ShapeFactory::createRandomShape();
    if (tempShape) {
        int shapeWidth = (tempShape->getWidth()) * BOX_SIZE; // Calculate total width of the shape in pixels

        // Debugging: Print the calculated shapeWidth
        Serial.print("shapeWidth = ");
        Serial.println(shapeWidth);

        // Calculate the center of the screen
        double screenCenterX = SCREEN_WIDTH / 2.0; // Division as double for precision
        Serial.print("center of screen = ");
        Serial.println(screenCenterX);

        // Get the x-coordinate of the main block (assumed to be in pixels)
        int mainBlockX = tempShape->blockList[0].getX()*BOX_SIZE; // Convert to pixels if necessary
        Serial.print("mainBlockX = ");
        Serial.println(mainBlockX);

        // Calculate the x-coordinate where the shape starts (assumed to be in pixels)
        int leftBlockX = mainBlockX+tempShape->getLeftBlock().getX()*BOX_SIZE; // Convert to pixels if needed
        Serial.print("leftBlockX = ");
        Serial.println(leftBlockX);

        // Calculate the x-coordinate where the shape starts (assumed to be in pixels)
        int rightBlockX = mainBlockX+tempShape->getRightBlock().getX()*BOX_SIZE; // Convert to pixels if needed
        Serial.print("rightBlockX = ");
        Serial.println(rightBlockX);

        // Calculate the x-coordinate of the center of the shape
        double shapeCenterX = (leftBlockX+rightBlockX)/2; // Center of the shape in pixels
        Serial.print("center of shape = ");
        Serial.println(shapeCenterX);

        // Calculate the offset needed to align the shape's center with the screen center
        int offset = (screenCenterX - shapeCenterX); // Convert to int for positioning
        Serial.print("offset = ");
        Serial.println(offset);

        tempShape->setBlock(Block(mainBlockX-offset, 60), 0); // Convert back to blocks if necessary and set new position
        tempShape->generateShape();
        Serial.print("new mainBlockX = ");
        Serial.println(mainBlockX-offset);

        // Draw the shape at the new position
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
    displayNavigation("Change", "Enter");
    currentChar = 'A'; 
    bool nameConfirmed = false;
    bool blinkState = true;
    unsigned long lastBlinkTime = 0;
    const unsigned long blinkInterval = 500;

    tft.setTextSize(2);
    drawHeader(); // Draw the "New Highscore" header initially
    drawName();

    int nameEntryY = 120;

    while (!nameConfirmed) {
        // Handle blinking
        if (millis() - lastBlinkTime >= blinkInterval) {
            blinkState = !blinkState;
            lastBlinkTime = millis();
            tft.setTextSize(2);
            drawCurrentChar(blinkState, nameEntryY);
        }

        // Handle button input for cycling characters
        if (digitalRead(BUTTON_LEFT) == LOW) {
            cycleCharacter();
            tft.setTextSize(2);
            drawCurrentChar(true, nameEntryY); 
            inputHandler.waitForButtonRelease(BUTTON_LEFT);
            delay(50);
        }

        // Handle button input for confirming character
        if (digitalRead(BUTTON_RIGHT) == LOW) {
            inputHandler.waitForButtonRelease(BUTTON_RIGHT);
            confirmCurrentCharacter();
            drawConfirmedChar(nameEntryY);
            currentCharIndex++;
            if (currentCharIndex >= MAX_NAME_LENGTH) {
                nameConfirmed = promptNameConfirmation();
            } else {
                currentChar = 'A';
            }
        }
    }
    return nameConfirmed;
}

const char* DisplayManager::getPlayerName() const {
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

    // Debug output
    Serial.print("Drawing confirmed character: ");
    Serial.print(playerName[currentCharIndex]);
    Serial.print(" at position (x, y): (");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(nameEntryY);
    Serial.println(")");

    tft.drawLine(x, nameEntryY + 20, x + 12, nameEntryY + 20, TFT_BLACK);
}

void DisplayManager::displayNavigation(const char* leftText, const char* rightText) {
    tft.setTextSize(1);
    int y = SCREEN_HEIGHT - 25; 
    int leftX = 25; 
    int rightX = SCREEN_WIDTH - (tft.textWidth(rightText) + 25); 

    tft.fillRect(leftX, y, (SCREEN_WIDTH - 40), 12, TFT_BLACK);

    tft.setCursor(leftX, y);
    tft.print(leftText);
    tft.setCursor(rightX, y);
    tft.print(rightText);
}

bool DisplayManager::promptNameConfirmation() {
    displayNavigation("Reset", "Enter");

    // Center and display "Press" and "a button"
    tft.setTextSize(2);
    const char* pressText = "Submit Name";
    int pressTextX = getCenteredX(pressText);
    tft.setCursor(pressTextX, 200);
    tft.println(pressText);
    while (true) {
        // Wait for button input
        if (digitalRead(BUTTON_RIGHT) == LOW) {
            inputHandler.waitForButtonRelease(BUTTON_RIGHT);
            return true;
        } else if (digitalRead(BUTTON_LEFT) == LOW) {
            inputHandler.waitForButtonRelease(BUTTON_LEFT);
            retryNameEntry();
            return false;
        }
    }
}

void DisplayManager::drawCurrentChar(bool highlight, int nameEntryY) {
    int x = 29 + currentCharIndex * 20;
    tft.fillRect(x, nameEntryY, 12, 20, TFT_BLACK);
    tft.setCursor(x, nameEntryY);
    tft.setTextColor(TFT_WHITE);
    tft.print(playerName[currentCharIndex]);
    tft.drawLine(x, nameEntryY + 20, x + 12, nameEntryY + 20, highlight ? TFT_BLACK : TFT_WHITE);
}

void DisplayManager::confirmCurrentCharacter() {
    playerName[currentCharIndex] = currentChar;

    // Debug output
    Serial.print("Confirmed Character: ");
    Serial.println(currentChar);
    Serial.print("Stored in playerName at index ");
    Serial.print(currentCharIndex);
    Serial.print(": ");
    Serial.println(playerName[currentCharIndex]);
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