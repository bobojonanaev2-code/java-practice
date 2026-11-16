#include "Triangle.h"
#include "../common/Console.h"
#include <iomanip>

void runTests();
namespace {
Triangle example() { return Triangle({0, 0}, {3, 0}, {0, 4}); }
Triangle readTriangle() {
    std::array<Point, 3> points{};
    std::cout << "Координаты каждой вершины вводите отдельной строкой: x y; десятичный разделитель — точка.\n";
    for (std::size_t i = 0; i < 3; ++i) {
        auto stream = console::input(std::string(1, static_cast<char>('A' + i)) + " (x y): ");
        if (!(stream >> points[i].x >> points[i].y) || !std::isfinite(points[i].x) || !std::isfinite(points[i].y))
            throw std::invalid_argument("Введите две конечные числовые координаты.");
        console::requireEnd(stream);
    }
    return Triangle(points[0], points[1], points[2]);
}
void printVertices(const Triangle& triangle) {
    const auto points = triangle.vertices();
    for (std::size_t i = 0; i < 3; ++i)
        std::cout << static_cast<char>('A' + i) << '(' << points[i].x << ", " << points[i].y << ") ";
    std::cout << '\n';
}
void printLengths(const std::string& title, const std::array<double, 3>& lengths) {
    std::cout << title << ": A=" << lengths[0] << "; B=" << lengths[1] << "; C=" << lengths[2] << '\n';
}
void metrics(const Triangle& triangle) {
    printVertices(triangle);
    std::cout << "Площадь: " << triangle.area() << "\nПериметр: " << triangle.perimeter() << '\n';
    printLengths("Высоты", triangle.heights());
    printLengths("Медианы", triangle.medians());
    printLengths("Биссектрисы", triangle.bisectors());
    std::cout << "Радиус вписанной окружности: " << triangle.inradius()
              << "\nРадиус описанной окружности: " << triangle.circumradius() << '\n';
    const auto center = triangle.circumcenter();
    std::cout << "Центр описанной окружности: (" << center.x << ", " << center.y << ")\n"
              << "По сторонам: " << toString(triangle.sideType())
              << "\nПо углам: " << toString(triangle.angleType()) << '\n';
}
void demo() {
    const auto triangle = example();
    metrics(triangle);
    std::cout << "Поворот на 90 градусов вокруг A:\n";
    printVertices(triangle.rotateAroundVertex(0, 90));
    std::cout << "Поворот на 90 градусов вокруг центра окружности:\n";
    printVertices(triangle.rotateAroundCircumcenter(90));
}
void menu() {
    auto first = example();
    auto second = Triangle({0, 4}, {0, 0}, {3, 0});
    std::cout << "Загружены два равных треугольника контрольного примера.\n";
    while (true) {
        try {
            std::cout << "\n1 — Ввести T1; 2 — Ввести T2; 3 — Показать вершины обоих\n"
                         "4 — Равенство T1 и T2; 5 — Все характеристики выбранного треугольника\n"
                         "6 — Повернуть выбранный треугольник; 0 — Выход\n";
            switch (console::integer("> ", 0, 6)) {
            case 0: return;
            case 1: first = readTriangle(); printVertices(first); break;
            case 2: second = readTriangle(); printVertices(second); break;
            case 3:
                std::cout << "T1: "; printVertices(first);
                std::cout << "T2: "; printVertices(second); break;
            case 4: std::cout << "Треугольники равны: " << first.isCongruentTo(second) << '\n'; break;
            case 5: {
                const auto& chosen = console::integer("Треугольник: 1 или 2: ", 1, 2) == 1 ? first : second;
                metrics(chosen); break;
            }
            case 6: {
                auto& chosen = console::integer("Треугольник: 1 или 2: ", 1, 2) == 1 ? first : second;
                const int center = console::integer("Центр: 1 — A; 2 — B; 3 — C; 4 — центр описанной окружности: ", 1, 4);
                const double angle = console::real("Угол в градусах (+ против часовой стрелки): ");
                chosen = center == 4 ? chosen.rotateAroundCircumcenter(angle) : chosen.rotateAroundVertex(center - 1, angle);
                std::cout << "Координаты после поворота:\n"; printVertices(chosen); break;
            }
            }
        } catch (const std::exception& error) {
            std::cout << "Ошибка: " << error.what() << "\nПовторите действие через меню.\n";
        }
    }
}
}
int main(int argc, char* argv[]) {
    console::setup();
    std::cout << std::setprecision(10);
    try {
        std::cout << "16.9. Треугольник — Анаев Бобожон Исмоилович\n";
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
