#include <cassert>

#include "myimage.h"

CMyImage::CMyImage(int _height, int _width)
    : CMatrix(_height, _width)
{}

CMyImage::CMyImage(const QImage& _qimage)
    : CMatrix(_qimage.height(), _qimage.width())
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
    return 0;
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
