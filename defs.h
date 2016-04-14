#pragma once

#include <type_traits>

template<typename E>
constexpr auto toUType(E _enumerator) noexcept {
    return static_cast<std::underlying_type_t<E>>(_enumerator);
}

namespace mycv {

enum class BorderEffect {
    Zero,
    Wrap,
    Clamp,
    Mirror
};

enum class PyramidLayerEnum {
    CurrentSigma,
    EffectiveSigma,
    Image
};

enum class Poi {
    X,
    Y,
    OperatorValue
};

} // mycv
