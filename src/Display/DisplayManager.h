#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>
#include <Model/Shapes/Shape.h>
#include "Config.h"

class DisplayManager {
public:
    DisplayManager(TFT_eSPI& tft);
    void drawScreen();
    void drawGrid();
    void drawCenteredText(bool isGameOver, int score);
    int getMoveDownSpeed(int level);
    uint16_t getBackgroundColor() const;
    void clearScreen();
    bool promptPlayerForName();
    const char* getPlayerName() const;
    void drawHeader();
    void drawName();
    void confirmCurrentCharacter();
    void cycleCharacter();
    void waitForButtonClick(int buttonPin);
    void drawConfirmedChar(int nameEntryY);
    bool promptNameConfirmation();
    void drawCurrentChar(bool highlight, int nameEntryY);
    int getCenteredX(const char* text) const;

private:
    TFT_eSPI& tft;
    uint16_t backgroundColor;
    char playerName[MAX_NAME_LENGTH];
    int currentCharIndex;
    char currentChar;
    void resetNameEntry();
};

#endif
