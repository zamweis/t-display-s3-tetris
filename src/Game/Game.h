#ifndef GAME_H
#define GAME_H

#include "TFT_eSPI.h"
#include "Display/DisplayManager.h"
#include "HighScore/HighScoreManager.h"
#include "Input/InputHandler.h"
#include "Game/BlockMap.h"
#include "Model/Shapes/Shape.h"
#include "TetrisAI/TetrisAI.h" // Include the AI header

class Game {
public:
    Game(TFT_eSPI& tft, DisplayManager& displayManager, HighScoreManager& highScoreManager, InputHandler& inputHandler);
    void setup();
    void loop();
    void runSimulation(int numSimulations);
    void runSimulationGridSearch();

private:
    // Member variables
    TFT_eSPI& tft;
    DisplayManager& displayManager;
    HighScoreManager& highScoreManager;
    InputHandler& inputHandler;
    BlockMap blockMap;
    Shape* shape = nullptr;      // Pointer to the current shape
    unsigned long lastMoveDownTime = 0; // Time tracking for gravity
    unsigned long lastAIMoveTime = 0;   // Time tracking for AI move interval
    unsigned long aiMoveInterval = 0; // Interval for AI moves
    int score = 0;               // Current game score
    int level = 1;               // Current game level
    int linesCleared = 0;        // Total lines cleared
    ButtonState leftButtonState = IDLE;
    ButtonState rightButtonState = IDLE;

    bool targetMoveSet = false;           // Flag to track if AI has set the target move
    TetrisAI::Move bestMove;              // Store the AI's best move for the current shape

    TetrisAI tetrisAI; // Instance of the Tetris AI

    // Main game methods
    void displayStartScreenLoop();
    void handleHighScoreDisplay();
    void updateShapePosition(unsigned long currentTime);
    void createNewShape();
    void resetGame();
    void handleGameOver();
    void finalizeShape();
    void handleHighScoreEntry();
    void updateScoreAndLevel(int clearedLines);

    // Input and movement handling
    void handleButtonState(ButtonState &state, int buttonPin, unsigned long currentTime, void (Shape::*moveFunc)(BlockMap &), void (Shape::*rotateFunc)(BlockMap &));

    // AI and movement logic
    bool executeAIStep(); // Executes the AI step
    bool calculateBestMove(); // Calculates the AI's best move
    bool alignShapeRotation(); // Aligns the shape to the desired rotation
    bool moveShapeToTargetColumn(); // Moves the shape horizontally to the target column
    void dropShape(); // Drops the shape to finalize placement
    void finalizeShapePlacement();
};

#endif // GAME_H
