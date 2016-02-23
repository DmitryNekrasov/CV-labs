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
        return 0.5;
    } else if (m_BorderEffect == BorderEffect::Wrap) {
        return 0.8;
    } else {
        return 0;
    }
}

BorderEffect CMyImage::getBorderEffect() const {
    return m_BorderEffect;
}

void CMyImage::setBorderEffect(BorderEffect _border_effect) {
    m_BorderEffect = _border_effect;
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
