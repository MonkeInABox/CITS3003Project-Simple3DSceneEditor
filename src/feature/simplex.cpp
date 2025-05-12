#include "simplex.hpp"
#include <cstdint>
#include <iostream>
#include <ostream>

int32_t floor(double x) {
    int32_t i = x;
    return x < i ? i - 1 : i;
}

struct IPoint;
struct Point {
    double x, y;
    Point(double x, double y) : x(x), y(y) {};
    Point(IPoint p);
    Point operator+(Point other) { return {x + other.x, y + other.y}; }
    Point operator+(double scalar) { return {x + scalar, y + scalar}; }
    Point operator-(Point other) { return {x - other.x, y - other.y}; }
    Point operator-(double scalar) { return {x - scalar, y - scalar}; }
    double dot(Point other) { return x * other.x + y * other.y; }
    friend std::ostream &operator<<(std::ostream &os, Point self) {
        return os << "Point(" << self.x << ", " << self.y << ')';
    }
};
struct IPoint {
    int32_t x, y;
    IPoint(int32_t x, int32_t y) : x(x), y(y) {};
    IPoint(Point p) : x(floor(p.x)), y(floor(p.y)) {}
};

Point::Point(IPoint p) : x(double(p.x)), y(double(p.y)) {}

constexpr double F = 0.3660254037844386;
Point skew(Point point) {
    double factor = (point.x + point.y) * F;
    return {point.x + factor, point.y + factor};
}

uint32_t hash(uint32_t x) {
    x = ((x >> 16) ^ x) * 0x45d9f3bu;
    x = ((x >> 16) ^ x) * 0x45d9f3bu;
    x = (x >> 16) ^ x;
    return x;
}

uint32_t hash(IPoint point, uint32_t seed) {
    return hash(hash(hash(point.x) + point.y) + seed);
}

constexpr double G = 0.21132486540518708;
Point unskew(Point point) {
    return {
        point.x - (point.x + point.y) * G,
        point.y - (point.x + point.y) * G,
    };
}

double grad(uint32_t hash, Point point) {
    uint32_t h = hash & 0b111;
    double u = h < 4 ? point.x : point.y;
    double v = h < 4 ? point.y : point.x;
    return ((h & 1) != 0 ? -u : u) + ((h & 2) != 0 ? -2. * v : 2. * v);
}
double Simplex::generate(double x, double y, uint32_t seed) {
    Point point = {x, y};
    Point skewed_point = skew(point);
    IPoint corner1 = IPoint(skewed_point);
    Point unskewed_corner1 = unskew(corner1);
    Point offset1 = point - unskewed_corner1;
    Point corner2_offset = offset1.x > offset1.y ? Point{1, 0} : Point{0, 1};
    Point offset2 = offset1 - corner2_offset + G;
    Point offset3 = offset1 - 1. + (2. * G);

    double n1, n2, n3;

    double t1 = 0.5 - offset1.dot(offset1);
    if (t1 < 0.) {
        n1 = 0;
    } else {
        t1 *= t1;
        n1 = t1 * t1 * grad(hash(corner1, seed), offset1);
    }
    double t2 = 0.5 - offset2.dot(offset2);
    if (t2 < 0.) {
        n2 = 0;
    } else {
        t2 *= t2;
        n2 = t2 * t2 * grad(hash(Point(corner1) + corner2_offset, seed), offset2);
    }
    double t3 = 0.5 - offset3.dot(offset3);
    if (t3 < 0.) {
        n3 = 0;
    } else {
        t3 *= t3;
        n3 = t3 * t3 * grad(hash(Point(corner1) + Point(1, 1), seed), offset3);
    }

    return 45. * (n1 + n2 + n3);
}
