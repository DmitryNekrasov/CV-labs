#include <iostream>

#include "matrix.h"

namespace mycv {

CMatrix::CMatrix()
    : m_IntensityMap(nullptr)
    , m_Height(0)
    , m_Width(0)
{}

CMatrix::CMatrix(int _height, int _width)
    : m_Height(_height)
    , m_Width(_width)
{
    initializeIntensityMap();
}

CMatrix::CMatrix(int _height, int _width, const double _array[])
    : CMatrix(_height, _width)
{
    std::copy(_array, _array + m_Height * m_Width, m_IntensityMap.get());
}

int CMatrix::getHeight() const {
    return m_Height;
}

int CMatrix::getWidth() const {
    return m_Width;
}

void CMatrix::set(int _row, int _col, double _value) {
    assert(inRange(_row, _col));
    m_IntensityMap[getIndex(_row, _col)] = _value;
}

double* CMatrix::begin() {
    return m_IntensityMap.get();
}

double* CMatrix::end() {
    return begin() + m_Height * m_Width;
}

const double* CMatrix::begin() const {
    return m_IntensityMap.get();
}

const double* CMatrix::end() const {
    return begin() + m_Height * m_Width;
}

void CMatrix::initializeIntensityMap() {
    m_IntensityMap = std::make_unique<double[]>(size_t(m_Height * m_Width));
}

} // mycv
