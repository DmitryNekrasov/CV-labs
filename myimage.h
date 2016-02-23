#ifndef CMYIMAGE_H
#define CMYIMAGE_H

#include <QImage>

#include "matrix.h"

class CMyImage : public CMatrix
{
public:

    CMyImage(int _height, int _width);
    CMyImage(const QImage& _qimage);

    virtual double get(int _row, int _col) const override;

    std::unique_ptr<QImage> toQImagePtr() const;

private:

    double  convertToDouble(int _intensity) const;
    int     convertToInt(double _intensity) const;
};

#endif // CMYIMAGE_H
