#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>
#include <Model/Shapes/Shape.h>
#include "Config.h"
#include "Input/InputHandler.h"

class DisplayManager {
public:
    DisplayManager(TFT_eSPI& tft);
    void drawScreen();
    void drawGrid();
    void displayStartScreen();
    void displayGameOverScreen(int score);
    int getMoveDownSpeed(int level);
    uint16_t getBackgroundColor() const;
    void clearScreen();
    bool promptPlayerForName();
    const char* getPlayerName() const;
    void drawHeader();
    void drawName();
    void confirmCurrentCharacter();
    void cycleCharacter();
    void drawConfirmedChar(int nameEntryY);
    bool promptNameConfirmation();
    void drawCurrentChar(bool highlight, int nameEntryY);
    int getCenteredX(const char* text) const;
    void retryNameEntry();
    void resetNameEntry();
    void displayNavigation(const char* leftText, const char* rightText);

private:
    TFT_eSPI& tft;
    uint16_t backgroundColor;
    InputHandler inputHandler;
    char playerName[MAX_NAME_LENGTH + 1] = {'A', 'A', 'A', 'A', 'A', 'A', '\0'};
    int currentCharIndex;
    char currentChar;
};

#endif
