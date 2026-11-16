#include "Rational.h"
#include <limits>
#include <numeric>
#include <stdexcept>

namespace {
constexpr auto limit = (std::numeric_limits<std::int64_t>::max)();
std::int64_t magnitude(std::int64_t x) { return x < 0 ? -x : x; }
void checkRange(std::int64_t x) {
    if (x == (std::numeric_limits<std::int64_t>::min)())
        throw std::overflow_error("Коэффициент вне диапазона [-INT64_MAX, INT64_MAX].");
}
std::int64_t checkedAdd(std::int64_t a, std::int64_t b) {
    if ((b > 0 && a > limit - b) || (b < 0 && a < -limit - b))
        throw std::overflow_error("Переполнение при сложении рациональных коэффициентов.");
    return a + b;
}
std::int64_t checkedMultiply(std::int64_t a, std::int64_t b) {
    if (b != 0 && magnitude(a) > limit / magnitude(b))
        throw std::overflow_error("Переполнение при умножении рациональных коэффициентов.");
    return a * b;
}
std::int64_t parseInteger(const std::string& text) {
    if (text.empty()) throw std::invalid_argument("Пустое целое число.");
    std::size_t pos = (text[0] == '+' || text[0] == '-') ? 1 : 0;
    if (pos == text.size()) throw std::invalid_argument("После знака нужны цифры.");
    for (std::size_t i = pos; i < text.size(); ++i)
        if (text[i] < '0' || text[i] > '9')
            throw std::invalid_argument("Коэффициент: целое число или дробь p/q без пробелов.");
    try { return std::stoll(text); }
    catch (const std::out_of_range&) { throw std::overflow_error("Целое число не помещается в int64_t."); }
}
}

Rational::Rational(std::int64_t numerator, std::int64_t denominator)
    : numerator_(numerator), denominator_(denominator) {
    if (denominator == 0) throw std::domain_error("Знаменатель не может быть нулём.");
    checkRange(numerator);
    checkRange(denominator);
    if (denominator_ < 0) { numerator_ = -numerator_; denominator_ = -denominator_; }
    const auto divisor = std::gcd(magnitude(numerator_), denominator_);
    numerator_ /= divisor;
    denominator_ /= divisor;
}
Rational Rational::parse(const std::string& text) {
    const auto slash = text.find('/');
    if (slash == std::string::npos) return Rational(parseInteger(text));
    return Rational(parseInteger(text.substr(0, slash)), parseInteger(text.substr(slash + 1)));
}
std::string Rational::toString() const {
    return std::to_string(numerator_) + (denominator_ == 1 ? "" : "/" + std::to_string(denominator_));
}
Rational Rational::operator-() const { return Rational(-numerator_, denominator_); }
Rational Rational::operator+(const Rational& other) const {
    // Use the least common denominator, reducing before multiplication.
    const auto common = std::gcd(denominator_, other.denominator_);
    const auto left = other.denominator_ / common;
    const auto right = denominator_ / common;
    const auto numerator = checkedAdd(checkedMultiply(numerator_, left),
                                      checkedMultiply(other.numerator_, right));
    const auto reduction = std::gcd(magnitude(numerator), common);
    return Rational(numerator / reduction,
                    checkedMultiply(right, other.denominator_ / reduction));
}
Rational Rational::operator-(const Rational& other) const { return *this + (-other); }
Rational Rational::operator*(const Rational& other) const {
    const auto g1 = std::gcd(magnitude(numerator_), other.denominator_);
    const auto g2 = std::gcd(magnitude(other.numerator_), denominator_);
    return Rational(checkedMultiply(numerator_ / g1, other.numerator_ / g2),
                    checkedMultiply(denominator_ / g2, other.denominator_ / g1));
}
Rational Rational::operator/(const Rational& other) const {
    if (other.isZero()) throw std::domain_error("Деление на нулевой коэффициент.");
    return *this * Rational(other.denominator_, other.numerator_);
}
bool Rational::operator==(const Rational& other) const {
    return numerator_ == other.numerator_ && denominator_ == other.denominator_;
}
