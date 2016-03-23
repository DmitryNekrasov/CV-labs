#include "qimagegenerator.h"

#include <QPainter>

#include "simple.h"

namespace mycv {

namespace qimg {

static int convertToInt(double _intensity) {
    return int(_intensity * 255);
}

QImagePtrT toQImagePtr(const CMyImage& _image) {
    auto qimage_ptr = std::make_unique<QImage>(_image.getWidth(), _image.getHeight(), QImage::Format_RGB32);
    for (int i = 0, ei = _image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = _image.getWidth(); j < ej; j++) {
            auto gray = convertToInt(_image.get(i, j));
            assert(gray >= 0 && gray < 256 && "NOT NORMALIZED VALUE");
            qimage_ptr->setPixel(j, i, qRgb(gray, gray, gray));
        }
    }
    return qimage_ptr;
}

static void drawAroundPoint(QPainter& _painter, int _x, int _y) {
    _painter.drawPoint(_x - 1, _y);
    _painter.drawPoint(_x, _y - 1);
    _painter.drawPoint(_x, _y + 1);
    _painter.drawPoint(_x + 1, _y);
}

QImagePtrT drawPoints(const CMyImage& _image, const poi::PointsT& _points) {

    auto qimage = toQImagePtr(_image);

    QPainter painter(qimage.get());
    painter.setPen(Qt::red);

    for (const auto& point : _points) {
        auto y = std::get<toUType(Poi::X)>(point);
        auto x = std::get<toUType(Poi::Y)>(point);
        drawAroundPoint(painter, x, y);
    }

    return qimage;
}

QImagePtrT mergeImageAndDrawLines(const CMyImage& _first_image, const poi::PointsT& _first_points,
                                  const CMyImage& _second_image, const poi::PointsT& _second_points,
                                  unsigned char _line_alpha /*  = 255 */)
{
    CMyImage merged_image(std::max(_first_image.getHeight(), _second_image.getHeight()),
                          _first_image.getWidth() + _second_image.getWidth());

    for (int i = 0, ei = _first_image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = _first_image.getWidth(); j < ej; j++) {
            merged_image.set(i, j, _first_image.get(i, j));
        }
    }

    auto offset = _first_image.getWidth();

    for (int i = 0, ei = _second_image.getHeight(); i < ei; i++) {
        for (int j = 0, ej = _second_image.getWidth(); j < ej; j++) {
            merged_image.set(i, j + offset, _second_image.get(i, j));
        }
    }

    auto qimage = toQImagePtr(merged_image);

    QPainter painter(qimage.get());

    for (size_t i = 0, ei = std::min(_first_points.size(), _second_points.size()); i < ei; i++) {
        auto x1 = std::get<toUType(Poi::Y)>(_first_points[i]);
        auto y1 = std::get<toUType(Poi::X)>(_first_points[i]);
        auto x2 = std::get<toUType(Poi::Y)>(_second_points[i]);
        auto y2 = std::get<toUType(Poi::X)>(_second_points[i]);

        int r = qrand() % 256, g = qrand() % 256, b = qrand() % 256;

        painter.setPen(QColor(r, g, b, _line_alpha));
        painter.drawLine(x1, y1, x2 + offset, y2);

        painter.setPen(QColor(r, g, b));
        drawAroundPoint(painter, x1, y1);
        drawAroundPoint(painter, x2 + offset, y2);
    }

    return qimage;
}

} // qimg

} // mycv
