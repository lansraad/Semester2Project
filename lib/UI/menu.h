#pragma once
#include "ui_characters.h"
#include "encoder.h"
#include "LCD1602.h"
#include <functional>

struct MenuItem {
    const char* itemName;
    std::function<void()> callback;
};

class Menu {
    LCD1602& _lcd;
    Encoder& _encoder;
    Thread* _updateMenuThread = nullptr;
    int _selected;
    int _cursorPosition;
    int _lastCount = 1;
    MenuItem* _items;
    int _numItems;
    static Menu* _activeMenu;
    bool _update;

public:
    Menu(LCD1602& lcd, Encoder& encoder);
    void drawMenu();
    void display(MenuItem* items, int numItems);
    void start();
    void stop();
    
    void executeSelected();
    void startThread();
};
