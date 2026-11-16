#include "Triangle.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {
constexpr double comparisonTolerance = 1e-9;
constexpr double degeneracyTolerance = 1e-12;
void finitePoint(Point point) {
    if (!std::isfinite(point.x) || !std::isfinite(point.y))
        throw std::invalid_argument("Координаты должны быть конечными числами.");
}
double positiveResult(double value) {
    if (!std::isfinite(value) || value <= 0)
        throw std::overflow_error("Геометрический результат вне поддерживаемого диапазона double.");
    return value;
}
double distance(Point first, Point second) { return std::hypot(first.x - second.x, first.y - second.y); }
bool close(double left, double right) {
    return std::abs(left - right) <= comparisonTolerance * (std::max)(std::abs(left), std::abs(right));
}
}
Triangle::Triangle(Point a, Point b, Point c) : vertices_{a, b, c}, sides_{}, local_{}, scale_(0), cross_(0), area_(0) {
    for (const auto& vertex : vertices_) finitePoint(vertex);
    sides_ = {distance(b, c), distance(c, a), distance(a, b)};
    for (double side : sides_)
        if (!std::isfinite(side) || side == 0)
            throw std::invalid_argument("Совпадающие вершины или непредставимая длина стороны.");
    scale_ = *std::max_element(sides_.begin(), sides_.end());
    for (double side : sides_)
        if (side / scale_ <= degeneracyTolerance)
            throw std::invalid_argument("Вершины слишком близки относительно размеров треугольника.");
    for (std::size_t i = 0; i < 3; ++i)
        local_[i] = {(vertices_[i].x - a.x) / scale_, (vertices_[i].y - a.y) / scale_};
    cross_ = local_[1].x * local_[2].y - local_[1].y * local_[2].x;
    if (std::abs(cross_) <= degeneracyTolerance)
        throw std::invalid_argument("Вырожденный или численно почти вырожденный треугольник.");
    area_ = positiveResult((std::abs(cross_) * 0.5 * scale_) * scale_);
    (void)perimeter();
}
double Triangle::perimeter() const { return positiveResult(sides_[0] + sides_[1] + sides_[2]); }
bool Triangle::isCongruentTo(const Triangle& other) const {
    auto left = sides_, right = other.sides_;
    std::sort(left.begin(), left.end());
    std::sort(right.begin(), right.end());
    for (std::size_t i = 0; i < 3; ++i) if (!close(left[i], right[i])) return false;
    return true;
}
std::array<double, 3> Triangle::heights() const {
    std::array<double, 3> result{};
    for (std::size_t i = 0; i < 3; ++i)
        result[i] = positiveResult((std::abs(cross_) / (sides_[i] / scale_)) * scale_);
    return result;
}
std::array<double, 3> Triangle::medians() const {
    std::array<double, 3> result{};
    for (std::size_t i = 0; i < 3; ++i) {
        const auto b = local_[(i + 1) % 3], c = local_[(i + 2) % 3];
        const Point midpoint{b.x / 2 + c.x / 2, b.y / 2 + c.y / 2};
        result[i] = positiveResult(distance(local_[i], midpoint) * scale_);
    }
    return result;
}
std::array<double, 3> Triangle::bisectors() const {
    std::array<double, 3> result{};
    for (std::size_t i = 0; i < 3; ++i) {
        const auto j = (i + 1) % 3, k = (i + 2) % 3;
        const double toJ = sides_[k] / scale_, toK = sides_[j] / scale_;
        const double ratio = toJ / (toJ + toK);
        const Point intersection{local_[j].x * (1 - ratio) + local_[k].x * ratio,
                                 local_[j].y * (1 - ratio) + local_[k].y * ratio};
        result[i] = positiveResult(distance(local_[i], intersection) * scale_);
    }
    return result;
}
double Triangle::inradius() const {
    const double normalizedPerimeter = sides_[0] / scale_ + sides_[1] / scale_ + sides_[2] / scale_;
    return positiveResult((std::abs(cross_) / normalizedPerimeter) * scale_);
}
Point Triangle::localCircumcenter() const {
    const auto b = local_[1], c = local_[2];
    const double b2 = b.x * b.x + b.y * b.y, c2 = c.x * c.x + c.y * c.y;
    return {(c.y * b2 - b.y * c2) / (2 * cross_), (b.x * c2 - c.x * b2) / (2 * cross_)};
}
Point Triangle::circumcenter() const {
    const auto center = localCircumcenter();
    const Point result{vertices_[0].x + center.x * scale_, vertices_[0].y + center.y * scale_};
    finitePoint(result);
    return result;
}
double Triangle::circumradius() const {
    const auto center = localCircumcenter();
    return positiveResult(std::hypot(center.x, center.y) * scale_);
}
Triangle::SideType Triangle::sideType() const {
    auto sides = sides_;
    std::sort(sides.begin(), sides.end());
    if (close(sides[0], sides[2])) return SideType::Equilateral;
    if (close(sides[0], sides[1]) || close(sides[1], sides[2])) return SideType::Isosceles;
    return SideType::Scalene;
}
Triangle::AngleType Triangle::angleType() const {
    auto sides = sides_;
    std::sort(sides.begin(), sides.end());
    const double a = sides[0] / sides[2], b = sides[1] / sides[2];
    const double sum = a * a + b * b;
    if (std::abs(sum - 1) <= comparisonTolerance) return AngleType::Right;
    return sum > 1 ? AngleType::Acute : AngleType::Obtuse;
}
Triangle Triangle::rotate(Point center, double degrees) const {
    if (!std::isfinite(degrees)) throw std::invalid_argument("Угол должен быть конечным числом.");
    const double radians = std::remainder(degrees, 360.0) * (std::acos(-1.0) / 180.0);
    const double sine = std::sin(radians), cosine = std::cos(radians);
    auto points = vertices_;
    for (auto& point : points) {
        const double x = point.x - center.x, y = point.y - center.y;
        point = {center.x + x * cosine - y * sine, center.y + x * sine + y * cosine};
        finitePoint(point);
    }
    return Triangle(points[0], points[1], points[2]);
}
Triangle Triangle::rotateAroundVertex(int vertexIndex, double degrees) const {
    if (vertexIndex < 0 || vertexIndex > 2) throw std::out_of_range("Индекс вершины: 0 (A), 1 (B), 2 (C).");
    return rotate(vertices_[static_cast<std::size_t>(vertexIndex)], degrees);
}
Triangle Triangle::rotateAroundCircumcenter(double degrees) const { return rotate(circumcenter(), degrees); }
std::string toString(Triangle::SideType type) {
    switch (type) {
    case Triangle::SideType::Equilateral: return "равносторонний (также равнобедренный)";
    case Triangle::SideType::Isosceles: return "равнобедренный";
    case Triangle::SideType::Scalene: return "разносторонний";
    }
    throw std::logic_error("Неизвестный тип по сторонам.");
}
std::string toString(Triangle::AngleType type) {
    switch (type) {
    case Triangle::AngleType::Right: return "прямоугольный";
    case Triangle::AngleType::Acute: return "остроугольный";
    case Triangle::AngleType::Obtuse: return "тупоугольный";
    }
    throw std::logic_error("Неизвестный тип по углам.");
}
