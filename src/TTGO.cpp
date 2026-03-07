/*

  _       _   _            ____
 | |     (_) | |  _   _   / ___|   ___
 | |     | | | | | | | | | |  _   / _ \
 | |___  | | | | | |_| | | |_| | | (_) |
 |_____| |_| |_|  \__, |  \____|  \___/
                  |___/

website:https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library
Written by Lewis he //https://github.com/lewisxhe
*/

#include "TTGO.h"

TTGOClass *TTGOClass::_ttgo = nullptr;

EventGroupHandle_t TTGOClass::_tpEvent = nullptr;

#ifdef NATIVE_BUILD
// Native build - include testable state machine logic
#include "StateMachine.cpp"
#endif

// State machine integration methods
void TTGOClass::runStateMachine() {
    if (stateMachine) {
        stateMachine->run();
    }
}

void TTGOClass::handleButtonPress() {
    if (stateMachine && button) {
        stateMachine->handleButtonPress();
    }
}

void TTGOClass::updateDisplay() {
    if (stateMachine) {
        stateMachine->updateDisplay();
    }
}

void TTGOClass::updatePowerManagement() {
    if (stateMachine) {
        stateMachine->updatePowerManagement();
    }
}
