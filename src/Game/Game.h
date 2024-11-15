#ifndef GAME_H
#define GAME_H

#include "TFT_eSPI.h"
#include "Display/DisplayManager.h"
#include "HighScore/HighScoreManager.h"
#include "Input/InputHandler.h"
#include "Game/BlockMap.h"
#include "Model/Shapes/Shape.h"
#include "TetrisAI/TetrisAI.h" // Add this line to include the AI header

class Game {
public:
    Game(TFT_eSPI& tft, DisplayManager& displayManager, HighScoreManager& highScoreManager, InputHandler& inputHandler);
    void setup();
    void loop();

private:
    TFT_eSPI& tft;
    DisplayManager& displayManager;
    HighScoreManager& highScoreManager;
    InputHandler& inputHandler;
    BlockMap blockMap;
    Shape* shape;
    unsigned long lastMoveDownTime;
    unsigned long lastAIMoveTime;
    unsigned long aiMoveInterval = 300; // Set the interval to 300ms (you can tweak this value)
    int score;
    int level;
    int linesCleared;
    ButtonState leftButtonState;
    ButtonState rightButtonState;

    bool targetMoveSet;           // Flag to track if AI has set the target move
    TetrisAI::Move bestMove;      // Store the AI's best move for the current shape

    TetrisAI ai; // Add this line to define the AI member variable

    void displayStartScreenLoop();
    void handleHighScoreDisplay();
    void updateShapePosition(unsigned long currentTime);
    void createNewShape();
    void resetGame();
    void handleGameOver();
    void handleHighScoreEntry();
    void updateScoreAndLevel(int clearedLines);
    void handleShapeMovement(unsigned long currentTime);
    void handleButtonState(ButtonState &state, int buttonPin, unsigned long currentTime, void (Shape::*moveFunc)(BlockMap &), void (Shape::*rotateFunc)(BlockMap &));
    void handleAIMovement(unsigned long currentTime);    
    
    bool aiInProgress; // New flag to track AI operations
};

#endif // GAME_H
