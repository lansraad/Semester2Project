#include "encoder.h"

Encoder::Encoder(PinName pinA, PinName pinB, PinName button) : _channelA(pinA, PullUp), _channelB(pinB, PullUp), _button(button, PullUp), _counter(0), _lastState(0), _currentState(0), _running(false), _debounceThread(osPriorityNormal, 512) {
    _channelA.rise(callback(this, &Encoder::encoderISR));
    _button.rise(callback(this, &Encoder::buttonISR));
    Encoder::reset();
    _debounceTimer.start();
}

int Encoder::getCount() {
    return _counter;
}

void Encoder::setCount(int count) {
    _counter = count;
}

int Encoder::getDirection() {
    return _direction;
}

void Encoder::encoderISR() {
    if (_semaphoreCount == 0) {
        _encoderSemaphore.release();
        _semaphoreCount++;
    }
}

void Encoder::buttonISR() {
    _buttonSemaphore.release();
}

void Encoder::debounceThread() {
    while (_running) {
        _encoderSemaphore.acquire();
        _semaphoreCount = 0;

        if (_debounceTimer.elapsed_time() > 50ms) {
            _currentState = (_channelB << 1) | _channelA;
            int grayCode  = (_lastState << 2) | _currentState;
            int decoded = (grayCode >> 1) ^ grayCode;
            _direction = (decoded & 1) ? -1 : 1; // Map to 1 or -1
            _counter += _direction;

            if (_counter < _minCount) {
                _counter = _minCount;
            }

            else if (_counter > _maxCount) {
                _counter = _maxCount;
            }

            _lastState = _currentState;
            _debounceTimer.reset();
        }
    }
}

void Encoder::buttonThread() {
    while (_running) {
        _buttonSemaphore.acquire();

        if (_debounceTimer.elapsed_time() > 50ms) {
            _debounceTimer.reset();

            _buttonCallback();
        }
    }
}

void Encoder::setRange(int min, int max) {
    Encoder::reset();
    _minCount = min;
    _maxCount = max;
}

void Encoder::reset() {
    _counter = 0;
    _direction = 0;
}

void Encoder::start() {
    if (!_running) {
        _counter = 0;
        _running = true;
        _debounceThread.start(callback(this, &Encoder::debounceThread));
        _buttonThread.start(callback(this, &Encoder::buttonThread));
    }
}

void Encoder::stop() {
    _running = false;
    _debounceThread.terminate();
    _buttonThread.terminate();
}

void Encoder::setButtonCallback(std::function<void()> callback) {
    _buttonCallback = callback;
}