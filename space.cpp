#include "space.h"

#include <algorithm>

namespace mycv {

CSpace::CSpace(size_t _x_size, size_t _y_size, size_t _scale_size, size_t _angle_size)
    : m_XSize(_x_size)
    , m_YSize(_y_size)
    , m_ScaleSize(_scale_size)
    , m_AngleSize(_angle_size)
    , m_Array(_x_size * _y_size * _scale_size * _angle_size)
{
    std::fill(m_Array.begin(), m_Array.end(), 0);
}

size_t CSpace::getXSize() const {
    return m_XSize;
}

size_t CSpace::getYSize() const {
    return m_YSize;
}

size_t CSpace::getScaleSize() const {
    return m_ScaleSize;
}

size_t CSpace::getAngleSize() const {
    return m_AngleSize;
}

double CSpace::get(size_t _x, size_t _y, size_t _scale, size_t _angle) const {
    return m_Array[getIndex(_x, _y, _scale, _angle)];
}

void CSpace::increase(size_t _x, size_t _y, size_t _scale, size_t _angle) {
    m_Array[getIndex(_x, _y, _scale, _angle)]++;
}

size_t CSpace::getIndex(size_t _x, size_t _y, size_t _scale, size_t _angle) const {
    return _x * m_YSize * m_ScaleSize * m_AngleSize +
            _y * m_ScaleSize * m_AngleSize +
            _scale * m_AngleSize + _angle;
}

} // mycv
