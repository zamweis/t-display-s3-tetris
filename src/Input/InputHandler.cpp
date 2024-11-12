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
    while (digitalRead(buttonPin) == HIGH) {}
}

void InputHandler::waitForButtonRelease(int buttonPin) {
    while (digitalRead(buttonPin) == LOW) {}
}

void InputHandler::waitForButtonPressed(int buttonPin) {
     // Wait for the button to be released if it is already pressed
    while (digitalRead(buttonPin) == LOW) {
        delay(10); // Add a small delay to avoid busy-waiting and for debounce purposes
    }

    // Wait for the button to be pressed
    while (digitalRead(buttonPin) == HIGH) {
        delay(10); // Add a small delay to avoid busy-waiting and for debounce purposes
    }
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
