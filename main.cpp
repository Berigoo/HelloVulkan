#include <iostream>
#include "src/Hk.h"


int main() {
    HkDevice device;
    XcbSurface surface(500, 800, (XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK), (XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY));

    surface.createSurface(device.getInstance());
}
