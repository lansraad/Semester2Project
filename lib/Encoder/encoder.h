#pragma once
#include "mbed.h"
#include <functional>

class Encoder {
    InterruptIn _channelA;
    DigitalIn _channelB;
    InterruptIn _button;
    int _counter;
    int8_t _lastState;
    int8_t _currentState;
    Timer _debounceTimer;
    Semaphore _encoderSemaphore;
    Semaphore _buttonSemaphore;
    int _semaphoreCount = 0;
    int _maxCount = 100;
    int _minCount = -100;
    int _direction;
    std::function<void()> _buttonCallback;

    void encoderISR();
    void buttonISR();
    void debounceThread();
    void buttonThread();
    Thread _buttonThread;
    Thread _debounceThread;
    bool _running;
    
public:
    Encoder(PinName pinA, PinName pinB, PinName button);
    int getCount();
    void start();
    void stop();
    void reset();
    void setRange(int min, int max);
    int getDirection();
    void setCount(int count);
    void setButtonCallback(std::function<void()> callback);
};
