#pragma once
#include "mbed.h"

class LCD1602 {
    DigitalOut _rs, _enable;
    PwmOut _backlight, _contrast;
    BusOut _dataPins;
    Mutex lcdMutex;

    int _column;
    int _row;

    int address(int column, int row);
    void character(int column, int row, int c);
    void writeByte(int value);
    void writeCommand(int command);
    void writeData(int data);

    void handleNewline();
    void incrementColumn();
    void incrementRow();
    void checkRowOverflow();
    
public:
    LCD1602(PinName rs, PinName e, PinName d4, PinName d5, PinName d6, PinName d7, PinName backlight, PinName contrast);

    void clear();
    void init();
    void write(const char* string);
    void setCursor(int column, int row);
    void writeChar(int data);
    void loadCustomChar(int index, const unsigned char*);
    void writeCustomChar(int index);
    void setBrightness(float brightness);
    float getBrightness();
    void setContrast(float contrast);
    float getContrast();
};