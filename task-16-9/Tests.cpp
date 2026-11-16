#include "Triangle.h"
#include "../common/TestSupport.h"
#include <algorithm>
#include <iostream>
#include <limits>

void runTests() {
    using tests::check;
    using tests::near;
    using tests::fails;
    const Triangle t({0, 0}, {3, 0}, {0, 4});
    near(t.area(), 6, "площадь");
    near(t.perimeter(), 12, "периметр");
    near(t.inradius(), 1, "вписанная окружность");
    near(t.circumradius(), 2.5, "описанная окружность");
    near(t.circumcenter().x, 1.5, "центр X");
    near(t.circumcenter().y, 2, "центр Y");
    const std::array<double, 3> expectedHeights{2.4, 3, 4};
    const std::array<double, 3> expectedMedians{2.5, std::sqrt(13.0), std::sqrt(18.25)};
    const std::array<double, 3> expectedBisectors{12 * std::sqrt(2.0) / 7, std::sqrt(11.25), 4 * std::sqrt(10.0) / 3};
    for (std::size_t i = 0; i < 3; ++i) {
        near(t.heights()[i], expectedHeights[i], "высота " + std::to_string(i));
        near(t.medians()[i], expectedMedians[i], "медиана " + std::to_string(i));
        near(t.bisectors()[i], expectedBisectors[i], "биссектриса " + std::to_string(i));
    }
    check(t.sideType() == Triangle::SideType::Scalene && t.angleType() == Triangle::AngleType::Right, "разносторонний прямоугольный");
    const Triangle iso({0, 0}, {1, 0}, {0, 1});
    check(iso.sideType() == Triangle::SideType::Isosceles && iso.angleType() == Triangle::AngleType::Right, "равнобедренный прямоугольный");
    const Triangle equilateral({0, 0}, {2, 0}, {1, std::sqrt(3.0)});
    check(equilateral.sideType() == Triangle::SideType::Equilateral && equilateral.angleType() == Triangle::AngleType::Acute, "равносторонний остроугольный");
    near(equilateral.area(), std::sqrt(3.0), "площадь равностороннего");
    const Triangle obtuse({0, 0}, {4, 0}, {1, 1});
    check(obtuse.angleType() == Triangle::AngleType::Obtuse, "тупоугольный");
    check(!t.isCongruentTo(iso), "неравные треугольники");
    check(t.isCongruentTo(Triangle({10, 20}, {13, 20}, {10, 24})), "равенство после переноса");
    check(t.isCongruentTo(Triangle({0, 0}, {-3, 0}, {0, 4})), "равенство после отражения");
    const auto vertices = t.vertices();
    std::array<int, 3> indices{0, 1, 2};
    do {
        const Triangle reordered(vertices[indices[0]], vertices[indices[1]], vertices[indices[2]]);
        check(t.isCongruentTo(reordered), "все перестановки вершин");
        near(reordered.circumcenter().x, 1.5, "центр при перестановке X");
        near(reordered.circumcenter().y, 2, "центр при перестановке Y");
    } while (std::next_permutation(indices.begin(), indices.end()));
    const auto aboutA = t.rotateAroundVertex(0, 90).vertices();
    near(aboutA[0].x, 0, "A X"); near(aboutA[0].y, 0, "A Y");
    near(aboutA[1].x, 0, "B X"); near(aboutA[1].y, 3, "B Y");
    near(aboutA[2].x, -4, "C X"); near(aboutA[2].y, 0, "C Y");
    const auto aboutCenter = t.rotateAroundCircumcenter(90).vertices();
    const std::array<Point, 3> expectedCenter{Point{3.5, 0.5}, Point{3.5, 3.5}, Point{-0.5, 0.5}};
    for (std::size_t i = 0; i < 3; ++i) {
        near(aboutCenter[i].x, expectedCenter[i].x, "поворот вокруг центра X");
        near(aboutCenter[i].y, expectedCenter[i].y, "поворот вокруг центра Y");
    }
    for (double degrees : {-450.0, -90.0, 0.0, 37.0, 90.0, 360.0, 720.0}) {
        for (int i = 0; i < 3; ++i) {
            const auto rotated = t.rotateAroundVertex(i, degrees);
            check(t.isCongruentTo(rotated), "равенство после поворота вокруг вершины");
            near(rotated.area(), t.area(), "сохранение площади");
            near(rotated.vertices()[i].x, vertices[i].x, "неподвижная вершина X");
            near(rotated.vertices()[i].y, vertices[i].y, "неподвижная вершина Y");
        }
        const auto rotated = t.rotateAroundCircumcenter(degrees);
        check(t.isCongruentTo(rotated), "равенство после поворота вокруг центра");
        near(rotated.circumcenter().x, 1.5, "неподвижный центр X");
        near(rotated.circumcenter().y, 2, "неподвижный центр Y");
    }
    const Triangle small({0, 0}, {3e-50, 0}, {0, 4e-50});
    near(small.area() / 1e-100, 6, "малый масштаб");
    check(small.angleType() == Triangle::AngleType::Right, "тип при малом масштабе");
    const Triangle large({0, 0}, {3e100, 0}, {0, 4e100});
    near(large.area() / 1e200, 6, "большой масштаб");
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();
    fails<std::invalid_argument>([] { (void)Triangle({0, 0}, {1, 1}, {2, 2}); }, "коллинеарные вершины");
    fails<std::invalid_argument>([] { (void)Triangle({0, 0}, {0, 0}, {1, 1}); }, "совпадающие вершины");
    fails<std::invalid_argument>([] { (void)Triangle({0, 0}, {1, 0}, {2, 1e-14}); }, "почти вырожденный");
    fails<std::invalid_argument>([&] { (void)Triangle({nan, 0}, {1, 0}, {0, 1}); }, "NaN");
    fails<std::invalid_argument>([&] { (void)Triangle({0, 0}, {inf, 0}, {0, 1}); }, "бесконечная координата");
    fails<std::invalid_argument>([&] { t.rotateAroundVertex(0, inf); }, "бесконечный угол");
    fails<std::invalid_argument>([&] { t.rotateAroundCircumcenter(nan); }, "NaN угол");
    fails<std::out_of_range>([&] { t.rotateAroundVertex(-1, 90); }, "отрицательный индекс");
    fails<std::out_of_range>([&] { t.rotateAroundVertex(3, 90); }, "индекс вне диапазона");
    fails<std::overflow_error>([] { (void)Triangle({0, 0}, {1e200, 0}, {0, 1e200}); }, "переполнение площади");
    fails<std::overflow_error>([] { (void)Triangle({0, 0}, {1e-200, 0}, {0, 1e-200}); }, "площадь ниже диапазона");
    std::cout << "Все проверки задания 16.9 пройдены.\n";
}
