#pragma once
#include "Monomial.h"
#include <map>
#include <string>
#include <utility>

class Polynomial {
public:
    explicit Polynomial(int variableCount, const std::vector<Monomial>& terms = {});
    int variableCount() const { return variableCount_; }
    bool isZero() const { return terms_.empty(); }
    std::vector<Monomial> monomials() const;
    Polynomial combineLikeTerms() const;
    bool contains(const Monomial& term) const;
    bool operator==(const Polynomial& other) const;
    Polynomial add(const Polynomial& other) const;
    Polynomial subtract(const Polynomial& other) const;
    Polynomial multiply(const Polynomial& other) const;
    // first = quotient, second = remainder
    std::pair<Polynomial, Polynomial> divideWithRemainder(const Polynomial& divisor) const;
    Polynomial quotient(const Polynomial& divisor) const;
    Polynomial remainder(const Polynomial& divisor) const;
    std::string toString() const;
private:
    struct LeadingFirst {
        bool operator()(const std::vector<int>& left, const std::vector<int>& right) const {
            return left > right; // Descending lexicographic order: x1, x2, ...
        }
    };
    int variableCount_;
    std::map<std::vector<int>, Rational, LeadingFirst> terms_;
    void merge(const Monomial& term);
    void checkCompatible(const Polynomial& other) const;
};
