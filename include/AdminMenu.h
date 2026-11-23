// AdminMenu.h
#ifndef ADMIN_MENU_H
#define ADMIN_MENU_H

#include "ParkingLot.h"

namespace AdminMenu {
    //Verify password first; After successful verification, enter the administrator submenu loop
    void run(ParkingLot& lot);
}

#endif