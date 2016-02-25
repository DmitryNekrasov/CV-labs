#include "simple.h"

namespace mycv {

namespace smpl {

double convertToDouble(int _intensity) {
    return double(_intensity) / 255;
}

int convertToInt(double _intensity) {
    return int(_intensity * 255);
}

} // smpl

} // mycv
