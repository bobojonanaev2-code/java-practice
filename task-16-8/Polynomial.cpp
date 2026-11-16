#include "Polynomial.h"
#include <sstream>
#include <stdexcept>

Polynomial::Polynomial(int variableCount, const std::vector<Monomial>& terms)
    : variableCount_(variableCount) {
    if (variableCount < 1) throw std::invalid_argument("Число переменных должно быть положительным.");
    for (const auto& term : terms) merge(term);
}
void Polynomial::merge(const Monomial& term) {
    if (term.powers().size() != static_cast<std::size_t>(variableCount_))
        throw std::invalid_argument("Количество степеней не совпадает с числом переменных.");
    if (term.coefficient().isZero()) return;
    const auto found = terms_.find(term.powers());
    if (found == terms_.end()) terms_.emplace(term.powers(), term.coefficient());
    else {
        const auto sum = found->second + term.coefficient();
        if (sum.isZero()) terms_.erase(found);
        else found->second = sum;
    }
}
std::vector<Monomial> Polynomial::monomials() const {
    std::vector<Monomial> result;
    for (const auto& term : terms_) result.emplace_back(term.second, term.first);
    return result;
}
Polynomial Polynomial::combineLikeTerms() const {
    // Canonical form is maintained by every constructor and operation.
    return Polynomial(variableCount_, monomials());
}
void Polynomial::checkCompatible(const Polynomial& other) const {
    if (variableCount_ != other.variableCount_)
        throw std::invalid_argument("Несовместимое количество переменных в многочленах.");
}
bool Polynomial::contains(const Monomial& term) const {
    if (term.powers().size() != static_cast<std::size_t>(variableCount_))
        throw std::invalid_argument("Неверное количество степеней.");
    const auto found = terms_.find(term.powers());
    return found != terms_.end() && found->second == term.coefficient();
}
bool Polynomial::operator==(const Polynomial& other) const {
    return variableCount_ == other.variableCount_ && terms_ == other.terms_;
}
Polynomial Polynomial::add(const Polynomial& other) const {
    checkCompatible(other);
    auto result = *this;
    for (const auto& term : other.monomials()) result.merge(term);
    return result;
}
Polynomial Polynomial::subtract(const Polynomial& other) const {
    checkCompatible(other);
    auto result = *this;
    for (const auto& term : other.terms_) result.merge(Monomial(-term.second, term.first));
    return result;
}
Polynomial Polynomial::multiply(const Polynomial& other) const {
    checkCompatible(other);
    Polynomial result(variableCount_);
    for (const auto& left : monomials())
        for (const auto& right : other.monomials()) result.merge(left.multiply(right));
    return result;
}
std::pair<Polynomial, Polynomial> Polynomial::divideWithRemainder(const Polynomial& divisor) const {
    checkCompatible(divisor);
    if (divisor.isZero()) throw std::domain_error("Деление на нулевой многочлен.");
    Polynomial quotient(variableCount_), remainder(variableCount_), pending = *this;
    const Monomial divisorLead(divisor.terms_.begin()->second, divisor.terms_.begin()->first);
    // Invariant: *this = divisor * quotient + remainder + pending.
    // Each iteration strictly decreases the leading exponent vector of pending.
    while (!pending.isZero()) {
        const Monomial lead(pending.terms_.begin()->second, pending.terms_.begin()->first);
        if (lead.isDivisibleBy(divisorLead)) {
            const auto factor = lead.divide(divisorLead);
            quotient.merge(factor);
            pending = pending.subtract(divisor.multiply(Polynomial(variableCount_, {factor})));
        } else {
            remainder.merge(lead);
            pending.terms_.erase(pending.terms_.begin());
        }
    }
    return {quotient, remainder};
}
Polynomial Polynomial::quotient(const Polynomial& divisor) const { return divideWithRemainder(divisor).first; }
Polynomial Polynomial::remainder(const Polynomial& divisor) const { return divideWithRemainder(divisor).second; }
std::string Polynomial::toString() const {
    if (isZero()) return "0";
    std::ostringstream output;
    bool first = true;
    for (const auto& term : terms_) {
        const bool negative = term.second.numerator() < 0;
        if (!first) output << (negative ? " - " : " + ");
        else if (negative) output << '-';
        const auto coefficient = negative ? -term.second : term.second;
        output << coefficient.toString();
        for (std::size_t i = 0; i < term.first.size(); ++i) {
            if (term.first[i] == 0) continue;
            output << "*x" << i + 1;
            if (term.first[i] != 1) output << '^' << term.first[i];
        }
        first = false;
    }
    return output.str();
}
