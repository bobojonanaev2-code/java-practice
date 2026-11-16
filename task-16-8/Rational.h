#pragma once
#include <cstdint>
#include <string>

// Exact reduced fraction. Supported numerator: [-INT64_MAX, INT64_MAX].
class Rational {
public:
    Rational(std::int64_t numerator = 0, std::int64_t denominator = 1);
    static Rational parse(const std::string& text);
    std::int64_t numerator() const { return numerator_; }
    std::int64_t denominator() const { return denominator_; }
    bool isZero() const { return numerator_ == 0; }
    std::string toString() const;
    Rational operator-() const;
    Rational operator+(const Rational& other) const;
    Rational operator-(const Rational& other) const;
    Rational operator*(const Rational& other) const;
    Rational operator/(const Rational& other) const;
    bool operator==(const Rational& other) const;
private:
    std::int64_t numerator_;
    std::int64_t denominator_;
};
