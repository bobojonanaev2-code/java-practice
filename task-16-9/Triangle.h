#pragma once
#include <array>
#include <string>

struct Point { double x; double y; };

class Triangle {
public:
    enum class SideType { Equilateral, Isosceles, Scalene };
    enum class AngleType { Right, Acute, Obtuse };
    Triangle(Point a, Point b, Point c);
    std::array<Point, 3> vertices() const { return vertices_; }
    std::array<double, 3> sides() const { return sides_; }
    double area() const { return area_; }
    double perimeter() const;
    bool isCongruentTo(const Triangle& other) const;
    std::array<double, 3> heights() const;
    std::array<double, 3> medians() const;
    std::array<double, 3> bisectors() const;
    double inradius() const;
    double circumradius() const;
    Point circumcenter() const;
    SideType sideType() const;
    AngleType angleType() const;
    Triangle rotateAroundVertex(int vertexIndex, double degrees) const;
    Triangle rotateAroundCircumcenter(double degrees) const;
private:
    std::array<Point, 3> vertices_;
    std::array<double, 3> sides_; // a = BC, b = CA, c = AB
    std::array<Point, 3> local_; // Relative to A, divided by the longest side.
    double scale_;
    double cross_;
    double area_;
    Triangle rotate(Point center, double degrees) const;
    Point localCircumcenter() const;
};
std::string toString(Triangle::SideType type);
std::string toString(Triangle::AngleType type);
