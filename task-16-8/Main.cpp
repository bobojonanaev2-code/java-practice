#include "Polynomial.h"
#include "../common/Console.h"
#include <limits>

void runTests();
namespace {
Polynomial exampleP() { return Polynomial(2, {{1, {2, 0}}, {3, {0, 1}}, {-2, {0, 1}}, {2, {0, 0}}}); }
Polynomial exampleQ() { return Polynomial(2, {{1, {1, 0}}, {1, {0, 0}}}); }
Monomial readMonomial(int variables) {
    auto stream = console::input("  Коэффициент и " + std::to_string(variables) + " степеней: ");
    std::string coefficient;
    if (!(stream >> coefficient)) throw std::invalid_argument("Не указан коэффициент.");
    std::vector<int> powers(static_cast<std::size_t>(variables));
    for (auto& power : powers)
        if (!(stream >> power) || power < 0)
            throw std::invalid_argument("Введите все степени: целые числа от 0 до INT_MAX.");
    console::requireEnd(stream);
    return Monomial(Rational::parse(coefficient), std::move(powers));
}
Polynomial readPolynomial() {
    // Limits keep accidental console input from exhausting memory.
    const int variables = console::integer("Количество переменных (1..100): ", 1, 100);
    const int count = console::integer("Количество одночленов (0..10000; 0 — нулевой многочлен): ", 0, 10000);
    std::cout << "Каждый одночлен — отдельная строка: p/q e1 e2 ...\n"
                 "Например, 3/2 2 1 означает 3/2*x1^2*x2. Целые коэффициенты тоже допустимы.\n";
    std::vector<Monomial> terms;
    for (int i = 0; i < count; ++i) {
        std::cout << "Одночлен " << i + 1 << ":\n";
        terms.push_back(readMonomial(variables));
    }
    return Polynomial(variables, terms);
}
void show(const Polynomial& p, const Polynomial& q) {
    std::cout << "P = " << p.toString() << "\nQ = " << q.toString() << '\n';
}
void divide(const Polynomial& p, const Polynomial& q) {
    const auto result = p.divideWithRemainder(q);
    std::cout << "Частное P / Q = " << result.first.toString()
              << "\nОстаток = " << result.second.toString() << '\n';
    std::cout << "P = Q * частное + остаток: "
              << (p == q.multiply(result.first).add(result.second)) << '\n';
}
void demo() {
    const auto p = exampleP(), q = exampleQ();
    show(p, q);
    std::cout << "P содержит x2: " << p.contains(Monomial(1, {0, 1})) << '\n'
              << "P + Q = " << p.add(q).toString() << '\n'
              << "P - Q = " << p.subtract(q).toString() << '\n'
              << "P * Q = " << p.multiply(q).toString() << '\n';
    divide(p, q);
}
void menu() {
    auto p = exampleP(), q = exampleQ();
    std::cout << "Загружен контрольный пример. Ввод можно заменить через меню.\n";
    show(p, q);
    while (true) {
        try {
            std::cout << "\n1 — Ввести P; 2 — Ввести Q; 3 — Показать P и Q\n"
                         "4 — Привести подобные; 5 — Принадлежность одночлена\n"
                         "6 — Равенство; 7 — P + Q; 8 — P - Q; 9 — P * Q\n"
                         "10 — Частное и остаток P / Q; 0 — Выход\n";
            switch (console::integer("> ", 0, 10)) {
            case 0: return;
            case 1: p = readPolynomial(); show(p, q); break;
            case 2: q = readPolynomial(); show(p, q); break;
            case 3: show(p, q); break;
            case 4: p = p.combineLikeTerms(); q = q.combineLikeTerms(); show(p, q); break;
            case 5: {
                const auto& chosen = console::integer("Проверять в P (1) или Q (2): ", 1, 2) == 1 ? p : q;
                const auto term = readMonomial(chosen.variableCount());
                std::cout << "Принадлежит: " << chosen.contains(term) << '\n';
                break;
            }
            case 6: std::cout << "P = Q: " << (p == q) << '\n'; break;
            case 7: std::cout << p.add(q).toString() << '\n'; break;
            case 8: std::cout << p.subtract(q).toString() << '\n'; break;
            case 9: std::cout << p.multiply(q).toString() << '\n'; break;
            case 10: divide(p, q); break;
            }
        } catch (const std::exception& error) {
            std::cout << "Ошибка: " << error.what() << "\nПовторите действие через меню.\n";
        }
    }
}
}
int main(int argc, char* argv[]) {
    console::setup();
    try {
        std::cout << "16.8. Многочлены — Анаев Бобожон Исмоилович\n";
        if (argc == 1) menu();
        else if (argc == 2 && std::string(argv[1]) == "--test") runTests();
        else if (argc == 2 && std::string(argv[1]) == "--demo") demo();
        else throw std::invalid_argument("Допустимые параметры: --test или --demo; без параметров — меню.");
    } catch (const console::EndOfInput&) {
        std::cout << "\nВвод завершён.\n";
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
