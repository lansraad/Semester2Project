#include "menu.h"

Menu::Menu(LCD1602& lcd, Encoder& encoder) : _lcd(lcd), _encoder(encoder), _selected(0), _cursorPosition(0) {
}

void Menu::start() {
    debug("> New Menu thread started\n");
    _updateMenuThread = new Thread;
    _updateMenuThread->start(callback(this, &Menu::drawMenu));
}

void Menu::stop() {
    if (_updateMenuThread != nullptr) {
        _updateMenuThread->terminate();
        delete _updateMenuThread;
        _updateMenuThread = nullptr; // Set to nullptr after deletion to avoid dangling pointer
        debug("> Menu thread terminated\n");
    }
}

void Menu::display(MenuItem* items, int numItems) {
    _items = items;
    _numItems = numItems;
    _cursorPosition = 0;
    _update = true;
    _encoder.setRange(0, _numItems - 1);
    debug("> Menu thread updated\n");
}

void Menu::drawMenu() {
    while (true) {
        ThisThread::sleep_for(10ms);
        if (_encoder.getCount() != _lastCount || _update) {
            _update = false;
            _lcd.loadCustomChar(0, up_arrow_empty);
            _lcd.loadCustomChar(1, down_arrow_full);
            _lcd.loadCustomChar(2, up_arrow_full);
            _lcd.loadCustomChar(3, down_arrow_empty);

            _lcd.clear();
            _cursorPosition = _encoder.getDirection() == 1 ? 1 : 0;
            _selected = _encoder.getCount();
            _lcd.setCursor(0, _cursorPosition);
            _lcd.write(_selected ? ">" : "^");
            debug("> Cursor: %d, Selected: %d (%s)\n", _cursorPosition, _selected, _items[_selected].itemName);

            if (_cursorPosition == 0) {
                _lcd.setCursor(1, 0);
                _lcd.write(_items[_selected].itemName);
                _lcd.setCursor(1, 1);
                _lcd.write(_items[_selected+1].itemName);
            } else {
                _lcd.setCursor(1, 0);
                _lcd.write(_items[_selected-1].itemName);
                _lcd.setCursor(1, 1);
                _lcd.write(_items[_selected].itemName);
            }

            _lcd.setCursor(15, 0);

            if (_selected == 0 || (_selected == 1 && _cursorPosition == 1)) {
                _lcd.writeCustomChar(0);
                _lcd.setCursor(15, 1);
                _lcd.writeCustomChar(1);
            } else if (_selected > _numItems -2 || _selected + !_cursorPosition > _numItems -2) {
                _lcd.writeCustomChar(2);
                _lcd.setCursor(15, 1);
                _lcd.writeCustomChar(3);
            } else {
                _lcd.writeCustomChar(2);
                _lcd.setCursor(15, 1);
                _lcd.writeCustomChar(1);
            }
        }
        _lastCount = _selected;
    }
}

void Menu::executeSelected() {
    if (_items[_selected].callback != nullptr) {
        _items[_selected].callback();
        }
}
