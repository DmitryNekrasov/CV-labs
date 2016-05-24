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
                           const desc::BlobsT& _first_blobs, const desc::BlobsT& _second_blobs,
                           const desc::AnglesT& _first_angles, const desc::AnglesT& _second_angles,
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
        auto x1 = _first_blobs[match.first].y;
        auto y1 = _first_blobs[match.first].x;
        auto x2 = _second_blobs[match.second].y;
        auto y2 = _second_blobs[match.second].x;

        auto radius1 = int(_first_blobs[match.first].sigma * sqrt(2.0));
        auto radius2 = int(_second_blobs[match.second].sigma * sqrt(2.0));

        int r = qrand() % 256, g = qrand() % 256, b = qrand() % 256;

        painter.setPen(QColor(r, g, b, _line_alpha));
        painter.drawLine(x1, y1, x2 + offset, y2);

        painter.setPen(QColor(r, g, b, _rect_alpha));
        drawTiltedSquare(painter, x1, y1, radius1 * 2, _first_angles[match.first]);
        drawTiltedSquare(painter, x2 + offset, y2, radius2 * 2, _second_angles[match.second]);

        painter.drawEllipse(x1 - radius1, y1 - radius1, radius1 * 2, radius1 * 2);
        painter.drawEllipse(x2 - radius2 + offset, y2 - radius2, radius2 * 2, radius2 * 2);

        painter.setPen(QColor(r, g, b, _point_alpha));
        drawAroundPoint(painter, x1, y1);
        drawAroundPoint(painter, x2 + offset, y2);
    }

    return qimage;
}

QImagePtrT showBlobs(const CMyImage& _image, const desc::BlobsT& _blobs, const desc::AnglesT& _angles) {
    auto qimage = toQImagePtr(_image);
    QPainter painter(qimage.get());
    size_t angle_index = 0;
    for (const auto& blob : _blobs) {
        int r = qrand() % 256, g = qrand() % 256, b = qrand() % 256;
        painter.setPen(QColor(r, g, b));
        auto x = blob.y;
        auto y = blob.x;
        auto radius = int(blob.sigma * sqrt(2.0));
        painter.drawEllipse(x - radius, y - radius, radius * 2, radius * 2);
        drawTiltedSquare(painter, x, y, radius * 2, _angles[angle_index++]);
    }
    return qimage;
}

QImagePtrT getPanorama(const CMyImage& _first_image, const CMyImage& _second_image,
                       const transform::TransformationT& _h)
{
    auto first_width = _first_image.getWidth();
    auto first_height = _first_image.getHeight();
    auto qimage_ptr = std::make_unique<QImage>(first_width * 3, first_height * 3, QImage::Format_RGB32);

    transform::TransformationT h;
    std::transform(_h.begin(), _h.end(), h.begin(), [&](const auto& _value) {
        return _value / _h[_h.size() - 1];
    });

    QTransform transform(h[4], h[1], h[7], h[3], h[0], h[6], h[5], h[2], h[8]);
    QTransform transfer(1, 0, 0,
                        0, 1, 0,
                        first_width, first_height, 1);

    auto result_transform = transform * transfer;

    QPainter painter(qimage_ptr.get());

    painter.drawImage(first_width, first_height, *toQImagePtr(_second_image));
    painter.setTransform(result_transform);
    painter.drawImage(0, 0, *toQImagePtr(_first_image));

    return qimage_ptr;
}

} // qimg

} // mycv
