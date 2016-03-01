#include "array.h"

namespace mycv {

CArray::CArray(int _size)
    : m_Size(_size)
{
    m_Array = std::make_unique<double[]>(m_Size);
}

int CArray::getSize() const {
    return m_Size;
}

double& CArray::operator[](int _index) const {
    return m_Array[_index];
}

} // mycv
