#include <Arduino.h>
#include "Config.h"
#include "Display/DisplayManager.h"
#include "Game/Game.h"
#include "HighScore/HighScoreManager.h"
#include "Input/InputHandler.h"
#include "Game/ShapeFactory.h"

// TFT and display setup (specific to your hardware)
TFT_eSPI tft = TFT_eSPI(); 
DisplayManager displayManager(tft);

Preferences preferences;  // Create a Preferences object
HighScoreManager highScoreManager(preferences);  // Pass the Preferences object to the constructor
InputHandler inputHandler(BUTTON_LEFT, BUTTON_RIGHT);  // Pass pin numbers as required by your constructor
Game game(tft, displayManager, highScoreManager, inputHandler);

void setup() {
    randomSeed(analogRead(0));
    Serial.begin(115200);
    tft.init();
    pinMode(BACKLIGHT_PIN, OUTPUT);
    analogWrite(BACKLIGHT_PIN, 100);
    tft.setRotation(0);
    game.setup();
}

void loop() {
    game.loop();
}
 