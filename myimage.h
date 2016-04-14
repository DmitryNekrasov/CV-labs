#pragma once

#include <iostream>

#include <QImage>

#include "array.h"
#include "defs.h"
#include "matrix.h"

namespace mycv {

class CMyImage : public CMatrix
{
public:
    CMyImage(int _height, int _width, BorderEffect _border_effect = BorderEffect::Mirror);
    CMyImage(const QImage& _qimage, BorderEffect _border_effect = BorderEffect::Mirror);

    double get(int _row, int _col) const;
    double get(double _y, double _x) const;

    BorderEffect getBorderEffect() const;
    void setBorderEffect(BorderEffect _border_effect);

    CMyImage getNormalize() const;

    friend CMyImage operator-(const CMyImage& _left, const CMyImage& _right);

private:
    std::pair<int, int> getClampIndices(int _row, int _col) const;
    std::pair<int, int> getMirrorIndices(int _row, int _col) const;
    std::pair<int, int> getWrapIndices(int _row, int _col) const;

private:
    BorderEffect m_BorderEffect;
};

inline double CMyImage::get(int _row, int _col) const {
    if (inRange(_row, _col)) {
        return CMatrix::get(_row, _col);
    }
    if (m_BorderEffect == BorderEffect::Clamp) {
        auto indices = getClampIndices(_row, _col);
        return CMatrix::get(indices.first, indices.second);
    } else if (m_BorderEffect == BorderEffect::Mirror) {
        auto indices = getMirrorIndices(_row, _col);
        return CMatrix::get(indices.first, indices.second);
    } else if (m_BorderEffect == BorderEffect::Wrap) {
        auto indices = getWrapIndices(_row, _col);
        return CMatrix::get(indices.first, indices.second);
    }
    return 0;
}

} // mycv
