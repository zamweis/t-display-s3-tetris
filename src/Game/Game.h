#ifndef GAME_H
#define GAME_H

#include "TFT_eSPI.h"
#include "Game/BlockMap.h"
#include "Game/ShapeFactory.h"
#include "Display/DisplayManager.h"
#include "HighScore/HighScoreManager.h"
#include "Input/InputHandler.h"

class Game {
public:
    Game(TFT_eSPI& tft, DisplayManager& displayManager, HighScoreManager& highScoreManager, InputHandler& inputHandler);
    void setup();
    void loop();

private:
    void displayStartScreenLoop();
    void handleHighScoreDisplay();
    void handleHighScoreEntry();
    void displayHighScoreAndStartScreenLoop();
    void resetGame();
    void handleGameOver();
    void handleShapeMovement(unsigned long currentTime);
    void updateShapePosition(unsigned long currentTime);
    void createNewShape();
    void updateScoreAndLevel(int clearedLines);
    void handleButtonState(ButtonState &state, int buttonPin, unsigned long currentTime, void (Shape::*moveFunc)(BlockMap &), void (Shape::*rotateFunc)(BlockMap &));

    unsigned long lastMoveDownTime;
    Shape* shape;
    BlockMap blockMap;
    int score;
    int level;
    int linesCleared;
    ButtonState leftButtonState;
    ButtonState rightButtonState;
    TFT_eSPI& tft;
    DisplayManager& displayManager;
    HighScoreManager& highScoreManager;
    InputHandler& inputHandler;
};

#endif
