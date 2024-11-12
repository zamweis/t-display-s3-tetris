#ifndef GAME_H
#define GAME_H

#include "TFT_eSPI.h"
#include "Shapes/BlockMap.h"
#include "ShapeFactory.h"
#include "DisplayManager.h"
#include "HighScoreManager.h"
#include "InputHandler.h"

class Game {
public:
    Game(TFT_eSPI& tft, DisplayManager& displayManager, HighScoreManager& highScoreManager, InputHandler& inputHandler);
    void setup();
    void loop();

private:
    void resetGame();
    void handleGameOver();
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
