#include <iostream>

#include <QPainter>

#include "array.h"
#include "core.h"
#include "defs.h"
#include "matrix.h"
#include "myimage.h"
#include "poi.h"
#include "simple.h"

int main() {

    QImage qimage_in("/Users/ScanNorOne/Desktop/mgu.png");
    mycv::CMyImage image_in(qimage_in);

    auto moravec_poi = mycv::poi::getMoravecPoi(image_in, 3, 3, 0.5);

    std::cout << moravec_poi.size() << std::endl;

    auto qimg = image_in.toQImagePtr();

    QPainter painter(qimg.get());
    painter.setPen(Qt::red);

    for (const auto& point : moravec_poi) {
        auto y = point.first;
        auto x = point.second;
        painter.drawPoint(x - 1, y);
        painter.drawPoint(x, y - 1);
        painter.drawPoint(x, y + 1);
        painter.drawPoint(x + 1, y);
    }

    qimg->save("/Users/ScanNorOne/Desktop/out.png");

    return 0;
}
