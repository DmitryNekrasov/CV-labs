#include "myimage.h"

#include <cassert>

#include "simple.h"

namespace mycv {

CMyImage::CMyImage(int _height, int _width, BorderEffect _border_effect /* = BorderEffect::Mirror */)
    : CMatrix(_height, _width)
    , m_BorderEffect(_border_effect)
{}

static double convertToDouble(int _intensity) {
    return double(_intensity) / 255;
}

CMyImage::CMyImage(const QImage& _qimage, BorderEffect _border_effect /* = BorderEffect::Mirror */)
    : CMyImage(_qimage.height(), _qimage.width(), _border_effect)
{
    for (int i = 0, ei = getHeight(); i < ei; i++) {
        for (int j = 0, ej = getWidth(); j < ej; j++) {
            auto gray = qGray(_qimage.pixel(j, i));
            set(i, j, convertToDouble(gray));
        }
    }
}

static CArray getBArray(double _x, double _y);

double CMyImage::get(double _y, double _x) const {

    assert(inRange(int(_y), int(_x)));

    int row = int(_y);
    int col = int(_x);

    double y = _y - row;
    double x = _x - col;

    auto b = getBArray(x, y);

    double result_intensity =
            b[0]  * get(row, col) +
            b[1]  * get(row, col + 1) +
            b[2]  * get(row + 1, col) +
            b[3]  * get(row + 1, col + 1) +
            b[4]  * get(row, col - 1) +
            b[5]  * get(row - 1, col) +
            b[6]  * get(row + 1, col - 1) +
            b[7]  * get(row - 1, col + 1) +
            b[8]  * get(row, col + 2) +
            b[9]  * get(row + 2, col) +
            b[10] * get(row - 1, col - 1) +
            b[11] * get(row + 1, col + 2) +
            b[12] * get(row + 2, col + 1) +
            b[13] * get(row - 1, col + 2) +
            b[14] * get(row + 2, col - 1) +
            b[15] * get(row + 2, col + 2);

    return std::max(0.0, std::min(1.0, result_intensity));
}

BorderEffect CMyImage::getBorderEffect() const {
    return m_BorderEffect;
}

void CMyImage::setBorderEffect(BorderEffect _border_effect) {
    m_BorderEffect = _border_effect;
}

void CMyImage::normalize() {
    auto minmax = std::minmax_element(begin(), end());
    auto min_intensity = *minmax.first;
    auto max_intensity = *minmax.second;
    std::transform(begin(), end(), begin(),
    [=](const auto& intensity) {
        return (intensity - min_intensity) / double(max_intensity - min_intensity);
    });
}

std::pair<int, int> CMyImage::getClampIndices(int _row, int _col) const {
    int res_row;
    int res_col;

    if (_row < 0) {
        res_row = 0;
    } else if (_row >= getHeight()) {
        res_row = getHeight() - 1;
    } else {
        res_row = _row;
    }

    if (_col < 0) {
        res_col = 0;
    } else if (_col >= getWidth()) {
        res_col = getWidth() - 1;
    } else {
        res_col = _col;
    }

    return std::make_pair(res_row, res_col);
}

std::pair<int, int> CMyImage::getMirrorIndices(int _row, int _col) const {
    int res_row;
    int res_col;

    if (_row < 0) {
        res_row = -_row - 1;
    } else if (_row >= getHeight()) {
        res_row = 2 * getHeight() - _row - 1;
    } else {
        res_row = _row;
    }

    if (_col < 0) {
        res_col = -_col - 1;
    } else if (_col >= getWidth()) {
        res_col = 2 * getWidth() - _col - 1;
    } else {
        res_col = _col;
    }

    return std::make_pair(res_row, res_col);
}

std::pair<int, int> CMyImage::getWrapIndices(int _row, int _col) const {
    int res_row;
    int res_col;

    if (_row < 0) {
        res_row = getHeight() + _row;
    } else if (_row >= getHeight()) {
        res_row = _row - getHeight();
    } else {
        res_row = _row;
    }

    if (_col < 0) {
        res_col = getWidth() + _col;
    } else if (_col >= getWidth()) {
        res_col = _col - getWidth();
    } else {
        res_col = _col;
    }

    return std::make_pair(res_row, res_col);
}

CArray getBArray(double _x, double _y) {

    CArray b(16);

    b[0]  = (_x - 1) * (_x - 2) * (_x + 1) * (_y - 1) * (_y - 2) * (_y + 1) / 4.0;
    b[1]  = _x * (_x + 1) * (_x - 2) * (_y - 1) * (_y - 2) * (_y + 1) / -4.0;
    b[2]  = _y * (_x - 1) * (_x - 2) * (_x + 1) * (_y + 1) * (_y - 2) / -4.0;
    b[3]  = _x * _y * (_x + 1) * (_x - 2) * (_y + 1) * (_y - 2) / 4.0;
    b[4]  = _x * (_x - 1) * (_x - 2) * (_y - 1) * (_y - 2) * (_y + 1) / -12.0;
    b[5]  = _y * (_x - 1) * (_x - 2) * (_x + 1) * (_y - 1) * (_y - 2) / -12.0;
    b[6]  = _x * _y * (_x - 1) * (_x - 2) * (_y + 1) * (_y - 2) / 12.0;
    b[7]  = _x * _y * (_x + 1) * (_x - 2) * (_y - 1) * (_y - 2) / 12.0;
    b[8]  = _x * (_x - 1) * (_x + 1) * (_y - 1) * (_y - 2) * (_y + 1) / 12.0;
    b[9]  = _y * (_x - 1) * (_x - 2) * (_x + 1) * (_y - 1) * (_y + 1) / 12.0;
    b[10] = _x * _y * (_x - 1) * (_x - 2) * (_y - 1) * (_y - 2) / 36.0;
    b[11] = _x * _y * (_x - 1) * (_x + 1) * (_y + 1) * (_y - 2) / -12.0;
    b[12] = _x * _y * (_x + 1) * (_x - 2) * (_y - 1) * (_y + 1) / -12.0;
    b[13] = _x * _y * (_x - 1) * (_x + 1) * (_y - 1) * (_y - 2) / -36.0;
    b[14] = _x * _y * (_x - 1) * (_x - 2) * (_y - 1) * (_y + 1) / -36.0;
    b[15] = _x * _y * (_x - 1) * (_x + 1) * (_y - 1) * (_y + 1) / 36.0;

    return b;
}

} // mycv
