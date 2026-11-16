#pragma once
#include <cmath>
#include <stdexcept>
#include <string>

namespace tests {
inline void check(bool condition, const std::string& label) {
    if (!condition) throw std::runtime_error("Проверка не пройдена: " + label);
}
inline void near(double actual, double expected, const std::string& label, double tolerance = 1e-9) {
    check(std::isfinite(actual) && std::abs(actual - expected) <= tolerance, label);
}
template<class Exception, class Action>
void fails(Action action, const std::string& label) {
    try { action(); }
    catch (const Exception&) { return; }
    throw std::runtime_error("Ожидалась ошибка: " + label);
}
}
