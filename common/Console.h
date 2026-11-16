#pragma once
#include <cmath>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace console {
struct EndOfInput {};
inline void setup() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::cout << std::boolalpha;
}
inline std::string line(const std::string& prompt) {
    std::cout << prompt;
    std::string result;
    if (!std::getline(std::cin, result)) throw EndOfInput{};
    return result;
}
inline std::istringstream input(const std::string& prompt) {
    std::istringstream stream(line(prompt));
    stream.imbue(std::locale::classic());
    return stream;
}
inline void requireEnd(std::istringstream& stream) {
    stream >> std::ws;
    if (!stream.eof()) throw std::invalid_argument("Лишние данные в строке.");
}
inline int integer(const std::string& prompt, int min, int max) {
    auto stream = input(prompt);
    int value = 0;
    if (!(stream >> value) || value < min || value > max)
        throw std::invalid_argument("Ожидается целое число от " + std::to_string(min) + " до " + std::to_string(max) + ".");
    requireEnd(stream);
    return value;
}
inline double real(const std::string& prompt) {
    auto stream = input(prompt);
    double value = 0;
    if (!(stream >> value) || !std::isfinite(value))
        throw std::invalid_argument("Ожидается конечное число; десятичный разделитель — точка.");
    requireEnd(stream);
    return value;
}
}
