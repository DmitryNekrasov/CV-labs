#include "array.h"

namespace mycv {

CArray::CArray(int _size)
    : m_Size(_size)
{
    m_Array = std::make_unique<double[]>(size_t(m_Size));
}

int CArray::getSize() const {
    return m_Size;
}

double& CArray::operator[](int _index) const {
    return m_Array[size_t(_index)];
}

} // mycv
