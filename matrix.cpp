#include <cassert>
#include <iostream>

#include "matrix.h"

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

CMatrix::CMatrix(int _height, int _width, double _array[])
    : CMatrix(_height, _width)
{
    std::copy(_array, _array + m_Height * m_Width, m_IntensityMap.get());
}

double CMatrix::get(int _row, int _col) const {
    assert(inRange(_row, _col));
    return m_IntensityMap[getIndex(_row, _col)];
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

bool CMatrix::inRange(int _row, int _col) const {
    return _row >= 0 && _row < int(m_Height) && _col >= 0 && _col < int(m_Width);
}

void CMatrix::initializeIntensityMap() {
    m_IntensityMap = std::make_unique<double[]>(size_t(m_Height * m_Width));
}

size_t CMatrix::getIndex(int _row, int _col) const {
    return size_t(_row * m_Width + _col);
}
