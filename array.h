#pragma once

#include <memory>

namespace mycv {

class CArray
{
public:
    CArray(size_t _size);

    size_t getSize() const;

    double& operator[](size_t _index) const;

    double* begin();
    double* end();
    const double* begin() const;
    const double* end() const;

private:
    size_t m_Size;
    std::unique_ptr<double[]> m_Array;
};

} // mycv
