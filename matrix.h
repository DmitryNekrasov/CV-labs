#pragma once

#include <memory>

class CMatrix
{
public:

    CMatrix();
    CMatrix(int _height, int _width);
    CMatrix(int _height, int _width, const double _array[]);

    double  get(int _row, int _col) const;
    int     getHeight()             const;
    int     getWidth()              const;

    void set(int _row, int _col, double _value);

protected:

    bool    inRange(int _row, int _col)     const;

    std::unique_ptr<double[]>   m_IntensityMap;

private:

    void    initializeIntensityMap();
    size_t  getIndex(int _row, int _col)    const;

    int                         m_Height;
    int                         m_Width;

};
