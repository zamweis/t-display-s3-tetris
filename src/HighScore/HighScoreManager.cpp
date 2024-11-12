#include "HighScoreManager.h"
#include "Config.h"

HighScoreManager::HighScoreManager(Preferences& preferences) : preferences(preferences) {
    loadHighScores();
}

void HighScoreManager::loadHighScores() {
    preferences.begin("highScores", true);
    bool isEmpty = true;

    for (int i = 0; i < 15; ++i) {
        String keyName = "name" + String(i);
        String keyScore = "score" + String(i);

        // Check if the score entry exists
        int score = preferences.getInt(keyScore.c_str(), -1);
        if (score != -1) {
            highScores[i].score = score;
            String storedName = preferences.getString(keyName.c_str(), "---");
            storedName.toCharArray(highScores[i].name, sizeof(highScores[i].name));
            isEmpty = false; // Found at least one score
        } else {
            // Assign default values if preferences are not set
            highScores[i].score = 0;
            strncpy(highScores[i].name, "---", sizeof(highScores[i].name));
        }
    }

    preferences.end();
}

void HighScoreManager::saveHighScores() {
    preferences.begin("highScores", false);
    for (int i = 0; i < 15; ++i) {
        String keyName = "name" + String(i);
        String keyScore = "score" + String(i);
        preferences.putInt(keyScore.c_str(), highScores[i].score);
        preferences.putString(keyName.c_str(), String(highScores[i].name));
    }
    preferences.end();
}

void HighScoreManager::updateHighScores(int newScore, const char* playerName) {
    loadHighScores();

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
    saveHighScores();
}

void HighScoreManager::displayHighScores(TFT_eSPI& tft) {
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

bool HighScoreManager::isHighScore(int score) {
    for (int i = 0; i < 15; ++i) {
        if (score > highScores[i].score) {
            return true;
        }
    }
    return false;
}
