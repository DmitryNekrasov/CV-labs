#pragma once

#include <memory>

namespace mycv {

class CArray
{
public:
    CArray(int _size);

    int getSize() const;

    double& operator[](int _index) const;

private:
    int m_Size;
    std::unique_ptr<double[]> m_Array;
};

} // mycv
