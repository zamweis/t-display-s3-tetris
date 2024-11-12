#ifndef HIGH_SCORE_MANAGER_H
#define HIGH_SCORE_MANAGER_H

#include <Preferences.h>
#include <TFT_eSPI.h>

struct HighScore {
    char name[7];
    int score;
};

class HighScoreManager {
public:
    HighScoreManager(Preferences& preferences);
    void loadHighScores();
    void saveHighScores();
    void updateHighScores(int newScore, const char* playerName);
    void displayHighScores(TFT_eSPI& tft);
    bool isHighScore(int score);

private:
    Preferences& preferences;
    HighScore highScores[15];
};

#endif
