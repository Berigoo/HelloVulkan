//
// Created by pach on 1/6/24.
//

#include "HkSurface.h"

void HkSurface::run() {
    while(1) {
        callback();
    }
}

void HkSurface::setLoop(std::function<void()> callback) {
    this->callback = callback;
}
