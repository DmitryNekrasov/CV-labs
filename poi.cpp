#include "poi.h"

#include <iostream>

#include "core.h"
#include "simple.h"

namespace mycv {

namespace poi {

static double getC(const CMyImage& _image, int _w_size, int _x, int _y, int _dx, int _dy) {

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
            auto s = std::numeric_limits<double>::max();
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

CMyImage applyHarris(const CMyImage& _image, int _w_size, double _k /* = 0.06 */) {

    auto sobel_dx = getSobelDx(_image);
    auto sobel_dy = getSobelDy(_image);

    CMyImage a(_image.getHeight(), _image.getWidth());
    CMyImage b(_image.getHeight(), _image.getWidth());
    CMyImage c(_image.getHeight(), _image.getWidth());

    auto w_half = _w_size / 2;

    for (int i = 0, ei = _image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = _image.getWidth(); j < ej; j++) {
            double sum_a = 0, sum_b = 0, sum_c = 0;
            for (int u = -w_half; u <= w_half; u++) {
                for (int v = -w_half; v <= w_half; v++) {
                    auto i_x = sobel_dx.get(i + u, j + v);
                    auto i_y = sobel_dy.get(i + u, j + v);
                    sum_a += i_x * i_x;
                    sum_b += i_x * i_y;
                    sum_c += i_y * i_y;
                }
            }
            a.set(i, j, sum_a);
            b.set(i, j, sum_b);
            c.set(i, j, sum_c);
        }
    }

    CMyImage result(_image.getHeight(), _image.getWidth());

    for (int i = 0, ei = _image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = _image.getWidth(); j < ej; j++) {
            auto harris = a.get(i, j) * c.get(i, j) - smpl::sqr(b.get(i, j)) -
                    _k * smpl::sqr(a.get(i, j) + c.get(i, j));
            result.set(i, j, harris);
        }
    }

    return result;
}

PointsT getPoi(const CMyImage& _image, double _threshold, int _p_size) {

    auto p_half = _p_size / 2;

    PointsT points;

    for (int i = 0, ei = _image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = _image.getWidth(); j < ej; j++) {
            bool is_correct = true;
            for (int px = -p_half; px <= p_half && is_correct; px++) {
                for (int py = -p_half; py <= p_half && is_correct; py++) {
                    if (px != 0 || py != 0) {
                        is_correct = _image.get(i, j) > _image.get(i + px, j + py);
                    }
                }
            }
            if (is_correct && _image.get(i, j) > _threshold) {
                points.emplace_back(i, j, _image.get(i, j));
            }
        }
    }

    return points;
}

PointsT filterPoint(const PointsT& _points, size_t _target_quantity) {

    PointsT filter_points(_points);

    int r = 1;

    while (filter_points.size() > _target_quantity) {
        filter_points.erase(std::remove_if(filter_points.begin(), filter_points.end(),
            [&](const auto& _point) {
                for (const auto& point : filter_points) {
                    auto dst = smpl::getDistance(std::get<toUType(Poi::X)>(_point), std::get<toUType(Poi::Y)>(_point),
                                                 std::get<toUType(Poi::X)>(point), std::get<toUType(Poi::Y)>(point));
                    if (dst < r && std::get<toUType(Poi::OperatorValue)>(_point) < std::get<toUType(Poi::OperatorValue)>(point)) {
                        return true;
                    }
                }
                return false;
            }), filter_points.end());
        r++;
    }

    return filter_points;
}

} // poi

} // mycv
