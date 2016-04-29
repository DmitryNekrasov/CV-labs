#include "qimagegenerator.h"

#include <cmath>

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

static void drawTiltedSquare(QPainter& _painter, int _x, int _y, int _size, double _angle) {

    auto half = _size / 2;

    auto x1 = int(-half * cos(_angle) - half * sin(_angle)) + _x;
    auto y1 = int(-half * sin(_angle) + half * cos(_angle)) + _y;

    auto x2 = int(half * cos(_angle) - half * sin(_angle)) + _x;
    auto y2 = int(half * sin(_angle) + half * cos(_angle)) + _y;

    auto x3 = int(half * cos(_angle) + half * sin(_angle)) + _x;
    auto y3 = int(half * sin(_angle) - half * cos(_angle)) + _y;

    auto x4 = int(-half * cos(_angle) + half * sin(_angle)) + _x;
    auto y4 = int(-half * sin(_angle) - half * cos(_angle)) + _y;

    auto x_arrow = int(half * cos(_angle)) + _x;
    auto y_arrow = int(half * sin(_angle)) + _y;

    _painter.drawLine(x1, y1, x2, y2);
    _painter.drawLine(x2, y2, x3, y3);
    _painter.drawLine(x3, y3, x4, y4);
    _painter.drawLine(x4, y4, x1, y1);
    _painter.drawLine(_x, _y, x_arrow, y_arrow);
}

QImagePtrT showDescriptors(const CMyImage& _first_image, const CMyImage& _second_image,
                           const poi::PointsT& _first_points, const poi::PointsT& _second_points,
                           const desc::AnglesT& _first_angles, const desc::AnglesT& _second_angles, int _grid_size,
                           const desc::MatchesT& _matches, unsigned char _point_alpha /* = 255 */,
                           unsigned char _line_alpha /* = 255 */, unsigned char _rect_alpha /* = 255 */)
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

    for (const auto& match : _matches) {
        auto x1 = std::get<toUType(Poi::Y)>(_first_points[match.first]);
        auto y1 = std::get<toUType(Poi::X)>(_first_points[match.first]);
        auto x2 = std::get<toUType(Poi::Y)>(_second_points[match.second]);
        auto y2 = std::get<toUType(Poi::X)>(_second_points[match.second]);

        int r = qrand() % 256, g = qrand() % 256, b = qrand() % 256;

        painter.setPen(QColor(r, g, b, _line_alpha));
        painter.drawLine(x1, y1, x2 + offset, y2);

        painter.setPen(QColor(r, g, b, _rect_alpha));
        drawTiltedSquare(painter, x1, y1, _grid_size, _first_angles[match.first]);
        drawTiltedSquare(painter, x2 + offset, y2, _grid_size, _second_angles[match.second]);

        painter.setPen(QColor(r, g, b, _point_alpha));
        drawAroundPoint(painter, x1, y1);
        drawAroundPoint(painter, x2 + offset, y2);
    }

    return qimage;
}

QImagePtrT showBlobs(const CMyImage& _image, const desc::BlobsT& _blobs) {
    auto qimage = toQImagePtr(_image);
    QPainter painter(qimage.get());
    painter.setPen(Qt::red);
    for (const auto& blob : _blobs) {
        auto x = blob.y;
        auto y = blob.x;
        auto radius = int(blob.sigma * sqrt(2.0));
        painter.drawEllipse(x - radius, y - radius, radius * 2, radius * 2);

    }
    return qimage;
}

} // qimg

} // mycv
