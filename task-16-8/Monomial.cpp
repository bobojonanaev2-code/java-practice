#include "Monomial.h"
#include <limits>
#include <stdexcept>
#include <utility>

Monomial::Monomial(Rational coefficient, std::vector<int> powers)
    : coefficient_(coefficient), powers_(std::move(powers)) {
    if (powers_.empty()) throw std::invalid_argument("Нужна хотя бы одна переменная.");
    for (int power : powers_)
        if (power < 0) throw std::invalid_argument("Степени должны быть неотрицательными.");
}
void Monomial::checkCompatible(const Monomial& other) const {
    if (powers_.size() != other.powers_.size())
        throw std::invalid_argument("Несовместимое количество переменных в одночленах.");
}
bool Monomial::isDivisibleBy(const Monomial& other) const {
    checkCompatible(other);
    if (other.coefficient_.isZero()) return false;
    for (std::size_t i = 0; i < powers_.size(); ++i)
        if (powers_[i] < other.powers_[i]) return false;
    return true;
}
Monomial Monomial::multiply(const Monomial& other) const {
    checkCompatible(other);
    auto powers = powers_;
    for (std::size_t i = 0; i < powers.size(); ++i) {
        if (powers[i] > (std::numeric_limits<int>::max)() - other.powers_[i])
            throw std::overflow_error("Переполнение степени одночлена.");
        powers[i] += other.powers_[i];
    }
    return Monomial(coefficient_ * other.coefficient_, std::move(powers));
}
Monomial Monomial::divide(const Monomial& other) const {
    if (!isDivisibleBy(other)) throw std::domain_error("Одночлен не делится на указанный одночлен.");
    auto powers = powers_;
    for (std::size_t i = 0; i < powers.size(); ++i) powers[i] -= other.powers_[i];
    return Monomial(coefficient_ / other.coefficient_, std::move(powers));
}
