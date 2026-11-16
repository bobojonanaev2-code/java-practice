#include "Polynomial.h"
#include "../common/TestSupport.h"
#include <iostream>
#include <limits>
#include <random>

void runTests() {
    using tests::check;
    using tests::fails;
    const Polynomial p(2, {{1, {2, 0}}, {3, {0, 1}}, {-2, {0, 1}}, {2, {0, 0}}});
    const Polynomial q(2, {{1, {1, 0}}, {1, {0, 0}}});
    const Polynomial zero(2);
    const Polynomial canonical(2, {{2, {0, 0}}, {1, {0, 1}}, {1, {2, 0}}});
    check(p == canonical && p == p.combineLikeTerms(), "приведение подобных, порядок членов");
    check(p.contains({1, {0, 1}}) && !p.contains({3, {0, 1}}), "принадлежность учитывает коэффициент");
    check(!p.contains({0, {0, 0}}) && !zero.contains({0, {0, 0}}), "нулевые члены не хранятся");
    check(Polynomial(2, {{7, {1, 1}}, {-7, {1, 1}}, {0, {0, 0}}}) == zero, "удаление нулей");
    check(p.add(q) == Polynomial(2, {{1, {2, 0}}, {1, {1, 0}}, {1, {0, 1}}, {3, {0, 0}}}), "сумма");
    check(p.subtract(q) == Polynomial(2, {{1, {2, 0}}, {-1, {1, 0}}, {1, {0, 1}}, {1, {0, 0}}}), "разность");
    const Polynomial product(2, {{1, {3, 0}}, {1, {2, 0}}, {1, {1, 1}}, {2, {1, 0}}, {1, {0, 1}}, {2, {0, 0}}});
    check(p.multiply(q) == product, "произведение");
    const auto division = p.divideWithRemainder(q);
    check(division.first == Polynomial(2, {{1, {1, 0}}, {-1, {0, 0}}}), "контрольное частное");
    check(division.second == Polynomial(2, {{1, {0, 1}}, {3, {0, 0}}}), "контрольный остаток");
    check(p.quotient(q) == division.first && p.remainder(q) == division.second, "отдельные методы деления");
    check(p == q.multiply(division.first).add(division.second), "инвариант деления");
    const auto exact = product.divideWithRemainder(q);
    check(exact.first == p && exact.second == zero, "деление без остатка");
    const Polynomial xy(2, {{1, {1, 0}}, {1, {0, 1}}}), y(2, {{1, {0, 1}}});
    const auto continued = xy.divideWithRemainder(y);
    check(continued.first == Polynomial(2, {{1, {0, 0}}}) &&
          continued.second == Polynomial(2, {{1, {1, 0}}}), "продолжение после переноса в остаток");
    check(p.subtract(p) == zero && p.multiply(zero) == zero, "нулевые результаты");
    const auto zeroDivision = zero.divideWithRemainder(p);
    check(zeroDivision.first == zero && zeroDivision.second == zero, "нулевое делимое");
    const Polynomial constant(2, {{2, {0, 0}}});
    check(constant.quotient(p).isZero() && constant.remainder(p) == constant, "меньшая степень делимого");
    check(p.quotient(constant).multiply(constant) == p, "дробное частное");
    check(Rational(2, -4) == Rational(-1, 2), "сокращение и знак");
    check(Rational(0, -5).toString() == "0", "нормализация нуля");
    check(Rational(1, 2) + Rational(1, 3) == Rational(5, 6), "сумма дробей");
    check(Rational(1, 2) - Rational(3, 2) == Rational(-1), "разность дробей");
    check(Rational(2, 3) * Rational(9, 4) == Rational(3, 2), "произведение дробей");
    check(Rational(2, 3) / Rational(-4, 5) == Rational(-5, 6), "частное дробей");
    check(Rational::parse("-10/20") == Rational(-1, 2) && Rational::parse("+12") == Rational(12), "разбор дробей");
    const Polynomial fractional(1, {{Rational(1, 2), {1}}, {Rational(1, 3), {1}}});
    check(fractional == Polynomial(1, {{Rational(5, 6), {1}}}), "приведение дробных коэффициентов");
    check(fractional.multiply(Polynomial(1, {{Rational(3, 5), {2}}})) == Polynomial(1, {{Rational(1, 2), {3}}}), "дробное произведение");
    const auto max = (std::numeric_limits<std::int64_t>::max)();
    check(Rational(max, 2) * Rational(2, max) == Rational(1), "сокращение до умножения");
    check(Rational(max, 3) + Rational(-max, 3) == Rational(0), "сокращение больших чисел");
    check(Rational(-max).toString() == "-9223372036854775807", "нижняя поддерживаемая граница");
    fails<std::overflow_error>([&] { (void)(Rational(max) + Rational(1)); }, "переполнение суммы");
    fails<std::overflow_error>([&] { (void)(Rational(-max) - Rational(1)); }, "переполнение отрицательной суммы");
    fails<std::overflow_error>([&] { (void)(Rational(max) * Rational(2)); }, "переполнение произведения");
    fails<std::overflow_error>([&] { (void)(Rational(1, max) * Rational(1, 2)); }, "переполнение знаменателя");
    fails<std::overflow_error>([] { Rational::parse("9223372036854775808"); }, "переполнение ввода");
    fails<std::overflow_error>([] { Rational::parse("-9223372036854775808"); }, "INT64_MIN не поддерживается");
    fails<std::domain_error>([] { (void)Rational(1, 0); }, "нулевой знаменатель");
    fails<std::domain_error>([] { (void)(Rational(1) / Rational(0)); }, "нулевой коэффициент-делитель");
    for (const auto* invalid : {"", "1/", "/2", "1/2/3", "1.5", "abc", "--1", "+"})
        fails<std::invalid_argument>([&] { Rational::parse(invalid); }, "неверный формат коэффициента");
    fails<std::domain_error>([&] { p.divideWithRemainder(zero); }, "нулевой многочлен-делитель");
    fails<std::invalid_argument>([] { (void)Monomial(1, {-1}); }, "отрицательная степень");
    fails<std::invalid_argument>([] { (void)Monomial(1, {}); }, "нет переменных");
    fails<std::invalid_argument>([] { (void)Polynomial(0); }, "нулевая размерность");
    fails<std::invalid_argument>([] { (void)Polynomial(2, {{1, {1}}}); }, "несогласованные степени");
    const Polynomial oneVariable(1);
    check(!(zero == oneVariable), "равенство учитывает количество переменных");
    fails<std::invalid_argument>([&] { p.add(oneVariable); }, "размерность суммы");
    fails<std::invalid_argument>([&] { p.subtract(oneVariable); }, "размерность разности");
    fails<std::invalid_argument>([&] { p.multiply(oneVariable); }, "размерность произведения");
    fails<std::invalid_argument>([&] { p.divideWithRemainder(oneVariable); }, "размерность деления");
    fails<std::invalid_argument>([&] { p.contains({1, {0}}); }, "размерность принадлежности");
    fails<std::overflow_error>([] {
        const Monomial huge(1, {(std::numeric_limits<int>::max)()});
        (void)huge.multiply(Monomial(1, {1}));
    }, "переполнение степени");
    // Deterministic varied examples: validate both the identity and the reduced remainder.
    std::mt19937 random(168);
    for (int iteration = 0; iteration < 100; ++iteration) {
        std::vector<Monomial> terms;
        for (int i = 0; i < 5; ++i)
            terms.emplace_back(static_cast<int>(random() % 7) - 3,
                               std::vector<int>{static_cast<int>(random() % 4), static_cast<int>(random() % 4), static_cast<int>(random() % 3)});
        const Polynomial dividend(3, terms);
        const Polynomial divisor(3, {{1, {1, 0, 0}}, {1, {0, 1, 0}}, {-1, {0, 0, 0}}});
        const auto result = dividend.divideWithRemainder(divisor);
        check(dividend == divisor.multiply(result.first).add(result.second), "случайное деление: равенство");
        for (const auto& term : result.second.monomials())
            check(!term.isDivisibleBy(divisor.monomials().front()), "остаток приведён относительно делителя");
    }
    std::cout << "Все проверки задания 16.8 пройдены (включая 100 делений от трёх переменных).\n";
}
