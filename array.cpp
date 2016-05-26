#include "array.h"

namespace mycv {

CArray::CArray(size_t _size)
    : m_Size(_size)
{
    m_Array = std::make_unique<double[]>(m_Size);
}

size_t CArray::getSize() const {
    return m_Size;
}

double& CArray::operator[](size_t _index) const {
    return m_Array[_index];
}

double* CArray::begin() {
    return m_Array.get();
}

double* CArray::end() {
    return m_Array.get() + m_Size;
}

const double* CArray::begin() const {
    return m_Array.get();
}
const double* CArray::end() const {
    return m_Array.get() + m_Size;
}

} // mycv
