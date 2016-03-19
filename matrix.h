#pragma once

#include <cassert>
#include <memory>

namespace mycv {

class CMatrix
{
public:
    CMatrix();
    CMatrix(int _height, int _width);
    CMatrix(int _height, int _width, const double _array[]);

    double get(int _row, int _col) const;
    void set(int _row, int _col, double _value);

    int getHeight() const;
    int getWidth() const;

    double* begin();
    double* end();
    const double* begin() const;
    const double* end() const;

protected:
    bool inRange(int _row, int _col) const;

private:
    void initializeIntensityMap();
    size_t getIndex(int _row, int _col) const;

private:
    std::unique_ptr<double[]> m_IntensityMap;
    int m_Height;
    int m_Width;

};

inline double CMatrix::get(int _row, int _col) const {
    assert(inRange(_row, _col));
    return m_IntensityMap[getIndex(_row, _col)];
}

inline bool CMatrix::inRange(int _row, int _col) const {
    return _row >= 0 && _row < int(m_Height) && _col >= 0 && _col < int(m_Width);
}

inline size_t CMatrix::getIndex(int _row, int _col) const {
    return size_t(_row * m_Width + _col);
}

} // mycv
