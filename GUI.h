#ifndef GUI_H_
#define GUI_H_

#include "MeshLib_Core/Point.h"

enum ProgStatus {
    PS_NORMAL,
    PS_PICKING_SEED
};

struct ColorManager {
    const static   Point COLOR_PICKED_FACE;
    const static   Point COLOR_NORMAL_FACE;
};



class GUI {

};

#endif