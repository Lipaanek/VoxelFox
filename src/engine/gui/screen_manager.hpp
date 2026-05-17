#pragma once
#include "screen.hpp"

class ScreenManager {
public:
    void SwitchTo(Screen* screen) {
        if (currentScreen && currentScreen != screen) {
            currentScreen->OnExit();
        }
        currentScreen = screen;
        if (currentScreen) {
            currentScreen->OnEnter();
        }
    }

    void UpdateCurrent() { if (currentScreen) currentScreen->Update(); }
    void RenderCurrent() { if (currentScreen) currentScreen->Render(); }
    Screen* GetCurrent() const { return currentScreen; }

private:
    Screen* currentScreen = nullptr;
};
