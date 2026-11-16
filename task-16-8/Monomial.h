#pragma once
#include "Rational.h"
#include <vector>

class Monomial {
public:
    Monomial(Rational coefficient, std::vector<int> powers);
    const Rational& coefficient() const { return coefficient_; }
    const std::vector<int>& powers() const { return powers_; }
    bool isDivisibleBy(const Monomial& other) const;
    Monomial multiply(const Monomial& other) const;
    Monomial divide(const Monomial& other) const;
private:
    Rational coefficient_;
    std::vector<int> powers_;
    void checkCompatible(const Monomial& other) const;
};
