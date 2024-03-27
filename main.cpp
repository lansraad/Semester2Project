#include "mbed.h"
#include "LCD1602.h"
#include "encoder.h"
#include "menu.h"

DigitalOut led(PC_11);
LCD1602 lcd(PB_14, PB_13, PC_6, PA_9, PA_8, PB_15, PC_7, PA_10); // reset, enable, d4-d7, backlight, contrast
Encoder encoder(PA_5, PA_4, PA_3);
Thread* backlightUpdateThread = nullptr;
Thread* contrastUpdateThread = nullptr;

void settingsMenu();

MenuItem home[] = {
    {"Home"},
    {"Settings", settingsMenu},
    {"None1"},
    {"None2"}
};

void homeMenu();
void backlight();
void contrast();

MenuItem settings[] = {
    {"Settings", homeMenu},
    {"Backlight", backlight},
    {"Contrast", contrast},
    {"None"}
};

Menu menu(lcd, encoder);

void homeMenu() {
    menu.stop();
    menu.start();
    menu.display(home, 4);
}

Ticker ledTicker;
Mutex lcdMutex;

void settingsMenu() {
    menu.stop();
    menu.start();
    menu.display(settings, 4);
}

void backlightThread() {
    debug("> Starting backlight updater thread\n");
    encoder.setRange(0, 100);
    int count = encoder.getCount();
    int lastCount = 1;
    encoder.setCount(lcd.getBrightness() * 100);

    while (true) {
        ThisThread::sleep_for(50ms);
        if (lastCount != encoder.getCount()) {
            lcdMutex.lock();
            lcd.clear();
            char buf[20];
            sprintf(buf, "Backlight: %d", encoder.getCount());
            lcd.write(buf);
            lcd.setBrightness(encoder.getCount() * 0.01);
            lastCount = encoder.getCount();
            lcdMutex.unlock();
        }
    }
}

void backlight() {
    menu.stop();

    if (backlightUpdateThread != nullptr) {
        backlightUpdateThread->terminate();
        delete backlightUpdateThread;
        backlightUpdateThread = nullptr;
    }

    backlightUpdateThread = new Thread;
    backlightUpdateThread->start(callback(&backlightThread));
    
    encoder.setButtonCallback([]() {
        if (backlightUpdateThread != nullptr) {
            backlightUpdateThread->terminate();
            delete backlightUpdateThread;
            backlightUpdateThread = nullptr;
         }

        encoder.setButtonCallback(std::bind(&Menu::executeSelected, &menu)); // Bind the Menu's executeSelected function as the callback
        menu.start();
        menu.display(settings, 4);
    });
}

void contrastThread() {
    debug("> Starting contrast updater thread\n");
    encoder.reset();
    encoder.setRange(0, 100);
    int count = encoder.getCount();
    int lastCount = 1;
    int brightness = 0;

    while (true) {
        ThisThread::sleep_for(20ms);
        if (lastCount != encoder.getCount()) {
            lcdMutex.lock();
            lcd.clear();
            char buf[20];
            sprintf(buf, "Contrast: %d", encoder.getCount());
            lcd.write(buf);
            lcd.setContrast(encoder.getCount() * 0.01);
            lastCount = encoder.getCount();
            lcdMutex.unlock();
        }
    }
}

void contrast() {
     menu.stop();

    if (contrastUpdateThread != nullptr) {
        contrastUpdateThread->terminate();
        delete contrastUpdateThread;
        contrastUpdateThread = nullptr;
    }

    contrastUpdateThread = new Thread;
    contrastUpdateThread->start(callback(&contrastThread));
    
    encoder.setButtonCallback([]() {
        if (contrastUpdateThread != nullptr) {
            contrastUpdateThread->terminate();
            delete contrastUpdateThread;
            contrastUpdateThread = nullptr;
         }

        encoder.setButtonCallback(std::bind(&Menu::executeSelected, &menu)); // Bind the Menu's executeSelected function as the callback
        menu.start();
        menu.display(settings, 4);
    });
}

int main() {
    debug("\n\n>> PROGRAM START\n");
    lcd.init();
    lcd.setBrightness(1);
    lcd.setContrast(0.35);
    encoder.start();
    menu.start();
    menu.display(home, 4);
    encoder.setButtonCallback(std::bind(&Menu::executeSelected, &menu)); // Bind the Menu's executeSelected function as the callback

    while (1) {
        ThisThread::sleep_for(10ms);
        led = !led;
    }
}