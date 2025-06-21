#pragma once

#include <cmath>
#include <ostream>

/**
 * A 3D vector class for physics calculations
 */
class Vector3D {
public:
    // Components
    double x, y, z;
    
    // Constructors
    Vector3D() : x(0.0), y(0.0), z(0.0) {}
    Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}
    
    // Copy constructor
    Vector3D(const Vector3D& other) = default;
    
    // Assignment operator
    Vector3D& operator=(const Vector3D& other) = default;
    
    // Vector addition
    Vector3D operator+(const Vector3D& rhs) const {
        return Vector3D(x + rhs.x, y + rhs.y, z + rhs.z);
    }
    
    // Vector subtraction
    Vector3D operator-(const Vector3D& rhs) const {
        return Vector3D(x - rhs.x, y - rhs.y, z - rhs.z);
    }
    
    // Scalar multiplication
    Vector3D operator*(double scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }
    
    // Scalar division
    Vector3D operator/(double scalar) const {
        return Vector3D(x / scalar, y / scalar, z / scalar);
    }
    
    // Compound assignment operators
    Vector3D& operator+=(const Vector3D& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
    
    Vector3D& operator-=(const Vector3D& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }
    
    Vector3D& operator*=(double scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    
    Vector3D& operator/=(double scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    
    // Dot product
    double dot(const Vector3D& rhs) const {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }
    
    // Cross product
    Vector3D cross(const Vector3D& rhs) const {
        return Vector3D(
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        );
    }
    
    // Magnitude (length) of the vector
    double magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    // Squared magnitude (faster when only comparing lengths)
    double magnitudeSquared() const {
        return x * x + y * y + z * z;
    }
    
    // Normalize the vector (make it unit length)
    Vector3D normalize() const {
        double mag = magnitude();
        if (mag > 0) {
            return Vector3D(x / mag, y / mag, z / mag);
        }
        return Vector3D();
    }
    
    // Distance between two points
    static double distance(const Vector3D& a, const Vector3D& b) {
        return (b - a).magnitude();
    }
    
    // Squared distance between two points (faster)
    static double distanceSquared(const Vector3D& a, const Vector3D& b) {
        return (b - a).magnitudeSquared();
    }
};

// Stream output operator
inline std::ostream& operator<<(std::ostream& os, const Vector3D& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

// Scalar multiplication (scalar * vector)
inline Vector3D operator*(double scalar, const Vector3D& v) {
    return v * scalar;
} 