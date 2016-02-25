#pragma once

#include <iostream>

#include <QImage>

#include "defs.h"
#include "matrix.h"

class CMyImage : public CMatrix
{
public:

    CMyImage(int _height, int _width, BorderEffect _border_effect = BorderEffect::Zero);
    CMyImage(const QImage& _qimage, BorderEffect _border_effect = BorderEffect::Zero);
    CMyImage(const CMyImage&) {std::cout << "COPY" << std::endl;}  // TODO костыль
    CMyImage(CMyImage&&) = default;

    virtual double get(int _row, int _col)  const override;

    BorderEffect    getBorderEffect()       const;

    void            setBorderEffect(BorderEffect _border_effect);
    void            normalize();

    std::unique_ptr<QImage> toQImagePtr()   const;

    CMyImage& operator=(CMyImage&&) = default;

private:

    double  convertToDouble(int _intensity) const;
    int     convertToInt(double _intensity) const;

    std::pair<int, int> getClampIndices(int _row, int _col)     const;
    std::pair<int, int> getMirrorIndices(int _row, int _col)    const;
    std::pair<int, int> getWrapIndices(int _row, int _col)      const;

    BorderEffect m_BorderEffect;
};
