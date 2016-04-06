#pragma once

#include <cassert>

#include <utility>

namespace mycv {

namespace algo {

template <class Iterator, class Compare>
inline std::pair<Iterator, Iterator>
twoElement(Iterator _first, Iterator _last, Compare _comp) {
    assert(_last - _first >= 2);
    auto first = _first;
    auto second = _first + 1;
    if (_comp(*second, *first)) {
        std::swap(first, second);
    }
    for (auto current = _first + 2; current != _last; ++current) {
        if (_comp(*current, *first)) {
            second = first;
            first = current;
        } else if (_comp(*current, *second)) {
            second = current;
        }
    }
    return std::make_pair(first, second);
}

} // algo

} // mycv
