#include "LCD1602.h"

LCD1602::LCD1602(PinName rs, PinName e, PinName d4, PinName d5,
                 PinName d6, PinName d7, PinName backlight, PinName contrast) : _rs(rs),
        _enable(e), _dataPins(d4, d5, d6, d7), _backlight(backlight), _contrast(contrast) {
            _contrast.period_us(100);
            _backlight.period_us(100);
}

void LCD1602::init() {
    lcdMutex.lock();
    _enable  = 1;
    _rs = 0;            // command mode

    // Wait for display to start
    wait_us(15000);

    // send "Display Settings" 3 times on startup & prepare to transition to 4-bit mode
    for (int i=0; i<3; i++) {
        writeByte(0x3);
        wait_us(1640);
    }

    // Set LCD to 4-bit mode
    writeByte(0x2);
    wait_us(40);

    writeCommand(0x28); // Function set 001 BW N F - -

    // Turn on display without cursor and without blinking
    writeCommand(0x0C);

    writeCommand(0x6);  // Set Cursor Direction and Display Shift

    clear();
    lcdMutex.unlock();
}

void LCD1602::setBrightness(float brightness) {
    _backlight = brightness;
}

float LCD1602::getBrightness() {
    return _backlight;
}

void LCD1602::setContrast(float contrast) {
    _contrast = contrast;
}

float LCD1602::getContrast() {
    return _contrast;
}

void LCD1602::character(int column, int row, int ch) {
    int addr = address(column, row);
    writeCommand(addr);
    writeData(ch);
}

void LCD1602::write(const char* str) {
    while (*str != '\0') {
        writeChar(static_cast<int>(*str));
        str++;
    }
}

void LCD1602::writeChar(int value) {
    if (value == '\n') {
        handleNewline();
    } else {
        character(_column, _row, value);
        incrementColumn();
        checkRowOverflow();
    }
}

void LCD1602::loadCustomChar(int index, const unsigned char* pattern) {
    writeCommand(0x40 + (index * 8)); // Set address for the character
    for (int i = 0; i < 8; i++) {
        writeData(pattern[i]); // Write the pattern to CGRAM
    }
}

void LCD1602::writeCustomChar(int index) {
    character(_column, _row, index);
    incrementColumn();
    checkRowOverflow();
}

void LCD1602::handleNewline() {
    _column = 0;
    _row++;
    checkRowOverflow();
}

void LCD1602::incrementColumn() {
    _column++;
    if (_column >= 16) {
        _column = 0;
        incrementRow();
    }
}

void LCD1602::incrementRow() {
    _row++;
    checkRowOverflow();
}

void LCD1602::checkRowOverflow() {
    if (_row >= 2) {
        _row = 0;
    }
}

void LCD1602::writeByte(int value) {
    lcdMutex.lock();
    _dataPins = value >> 4; // Send the higher 4 bits of the value to the data pins
    wait_us(40);            // Wait for the display
    _enable = 0;            
    wait_us(40);
    _enable = 1;
    _dataPins = value >> 0; // Send the lower 4 bits of the value to the data pins
    wait_us(40);
    _enable = 0;
    wait_us(40);
    _enable = 1;
    lcdMutex.unlock();
}

void LCD1602::writeCommand(int command) {
    _rs = 0;
    writeByte(command);
}

void LCD1602::writeData(int data) {
    _rs = 1;
    writeByte(data);
}

int LCD1602::address(int column, int row) {
    return 0x80 + (row * 0x40) + column;
}

void LCD1602::setCursor(int column, int row) {
    _column = column;
    _row = row;
}

void LCD1602::clear() {
    writeCommand(0x01); // Clear the display
    wait_us(1640);      // Wait for the display (1.64 ms)
    setCursor(0, 0);    // Reset the cursor position
}