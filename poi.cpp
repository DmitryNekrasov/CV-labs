#include <iostream>

#include "poi.h"

namespace mycv {

namespace poi {

double getC(const CMyImage& _image, int _w_size, int _x, int _y, int _dx, int _dy) {

    double sum = 0;
    auto w_half = _w_size / 2;

    for (int u = -w_half; u <= w_half; u++) {
        for (int v = -w_half; v <= w_half; v++) {
            auto tmp =  _image.get(_x + u, _y + v) - _image.get(_x + u + _dx, _y + v + _dy);
            sum += tmp * tmp;
        }
    }

    return sum;
}

CMyImage applyMoravec(const CMyImage& _image, int _w_size) {

    CMyImage result(_image.getHeight(), _image.getWidth());

    for (int i = 0, ei = _image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = _image.getWidth(); j < ej; j++) {
            double s = 10;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx != 0 && dy != 0) {
                        auto c = getC(_image, _w_size, i, j, dx, dy);
                        s = std::min(s, c);
                    }
                }
            }
            result.set(i, j, s);
        }
    }

    return result;
}

PointsT getMoravecPoi(const CMyImage& _image, int _w_size, int _p_size, double _threshold) {

    auto s = applyMoravec(_image, _w_size);
    auto p_half = _p_size / 2;

    PointsT points;

    for (int i = 0, ei = s.getHeight(); i < ei; i++) {
        for (int j = 0, ej = s.getWidth(); j < ej; j++) {
            bool is_correct = true;
            for (int px = -p_half; px <= p_half && is_correct; px++) {
                for (int py = -p_half; py <= p_half && is_correct; py++) {
                    is_correct = s.get(i, j) >= s.get(i + px, j + py);
                }
            }
            if (is_correct && s.get(i, j) > _threshold) {
                points.emplace_back(i, j);
            }
        }
    }

    return points;
}

} // poi

} // mycv
