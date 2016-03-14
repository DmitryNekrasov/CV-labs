#include "qimagegenerator.h"

#include <QPainter>

namespace mycv {

namespace qimg {

QImagePtrT drawPoints(const CMyImage& _image, const poi::PointsT& _points) {

    auto qimage = std::make_unique<QImage>(*_image.toQImagePtr());

    QPainter painter(qimage.get());
    painter.setPen(Qt::red);

    for (const auto& point : _points) {
        auto y = point.first;
        auto x = point.second;
        painter.drawPoint(x - 1, y);
        painter.drawPoint(x, y - 1);
        painter.drawPoint(x, y + 1);
        painter.drawPoint(x + 1, y);
    }

    return qimage;
}

} // qimg

} // mycv
