#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

class InputHandler {
public:
    InputHandler(int leftPin, int rightPin);
    bool isButtonPressed(int buttonPin);
    bool isButtonReleased(int buttonPin);
    void waitForButtonClick(int buttonPin);
    void waitForButtonRelease(int buttonPin);
    void startButtonPressTimer(unsigned long currentTime);
    unsigned long getButtonPressStart() const;
    unsigned long getLastRotateTime() const;
    void updateLastRotateTime(unsigned long time);;

private:
    int leftPin;
    int rightPin;
    unsigned long buttonPressStart;
    unsigned long lastRotateTime;
};

#endif
