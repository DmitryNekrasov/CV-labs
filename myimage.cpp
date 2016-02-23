#include <cassert>

#include "myimage.h"

CMyImage::CMyImage(int _height, int _width, BorderEffect _border_effect /* = BorderEffect::Zero */)
    : CMatrix(_height, _width)
    , m_BorderEffect(_border_effect)
{}

CMyImage::CMyImage(const QImage& _qimage, BorderEffect _border_effect /* = BorderEffect::Zero */)
    : CMyImage(_qimage.height(), _qimage.width(), _border_effect)
{
    for (int i = 0; i < getHeight(); i++) {
        for (int j = 0; j < getWidth(); j++) {
            auto gray = qGray(_qimage.pixel(j, i));
            set(i, j, convertToDouble(gray));
        }
    }
}

double CMyImage::get(int _row, int _col) const {
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

BorderEffect CMyImage::getBorderEffect() const {
    return m_BorderEffect;
}

void CMyImage::setBorderEffect(BorderEffect _border_effect) {
    m_BorderEffect = _border_effect;
}

void CMyImage::normalize() {
    auto minmax = std::minmax_element(m_IntensityMap.get(), m_IntensityMap.get() + getHeight() * getWidth());
    auto min_intensity = *minmax.first;
    auto max_intensity = *minmax.second;
    std::transform(m_IntensityMap.get(), m_IntensityMap.get() + getHeight() * getWidth(), m_IntensityMap.get(), [=](const auto& intensity) {
        return (intensity - min_intensity) / double(max_intensity - min_intensity);
    });
}

std::unique_ptr<QImage> CMyImage::toQImagePtr() const {
    auto qimage_ptr = std::make_unique<QImage>(getWidth(), getHeight(), QImage::Format_RGB32);
    for (int i = 0; i < getHeight(); i++) {
        for (int j = 0; j < getWidth(); j++) {
            auto gray = convertToInt(get(i, j));
            assert(gray >= 0 && gray < 256 && "NOT NORMALIZED VALUE");
            qimage_ptr->setPixel(j, i, qRgb(gray, gray, gray));
        }
    }
    return qimage_ptr;
}

double CMyImage::convertToDouble(int _intensity) const {
    return double(_intensity) / 255;
}

int CMyImage::convertToInt(double _intensity) const {
    return int(_intensity * 255);
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
        res_row = -_row;
    } else if (_row >= getHeight()) {
        res_row = 2 * getHeight() - _row - 1;
    } else {
        res_row = _row;
    }

    if (_col < 0) {
        res_col = -_col;
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
