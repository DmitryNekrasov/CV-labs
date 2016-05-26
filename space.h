#pragma once

#include <memory>

#include "array.h"

namespace mycv {

class CSpace
{
public:
    CSpace(size_t _x_size, size_t _y_size, size_t _scale_size, size_t _angle_size);

    size_t getXSize() const;
    size_t getYSize() const;
    size_t getScaleSize() const;
    size_t getAngleSize() const;

    double get(size_t _x, size_t _y, size_t _scale, size_t _angle) const;

    void increase(size_t _x, size_t _y, size_t _scale, size_t _angle);

private:
    size_t getIndex(size_t _x, size_t _y, size_t _scale, size_t _angle) const;

private:
    const size_t m_XSize;
    const size_t m_YSize;
    const size_t m_ScaleSize;
    const size_t m_AngleSize;

    CArray m_Array;
};

} // mycv
