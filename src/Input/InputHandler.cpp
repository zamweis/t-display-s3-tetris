#include "InputHandler.h"
#include <Arduino.h>

InputHandler::InputHandler(int leftPin, int rightPin) : leftPin(leftPin), rightPin(rightPin), buttonPressStart(0), lastRotateTime(0) {}

bool InputHandler::isButtonPressed(int buttonPin) {
    return digitalRead(buttonPin) == LOW;
}

bool InputHandler::isButtonReleased(int buttonPin) {
    return digitalRead(buttonPin) == HIGH;
}

void InputHandler::waitForButtonClick(int buttonPin) {
    while (digitalRead(buttonPin) == HIGH) {}
    while (digitalRead(buttonPin) == LOW) {}
}

void InputHandler::startButtonPressTimer(unsigned long currentTime) {
    buttonPressStart = currentTime;
}

unsigned long InputHandler::getButtonPressStart() const {
    return buttonPressStart;
}

unsigned long InputHandler::getLastRotateTime() const {
    return lastRotateTime;
}

void InputHandler::updateLastRotateTime(unsigned long time) {
    lastRotateTime = time;
}