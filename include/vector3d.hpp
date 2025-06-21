#pragma once

#include <cmath>
#include <ostream>

/**
 * A 3D vector class for physics calculations
 * Optimized with inline methods and move semantics
 */
class Vector3D {
public:
    // Components
    double x, y, z;
    
    // Constructors
    constexpr Vector3D() noexcept : x(0.0), y(0.0), z(0.0) {}
    constexpr Vector3D(double x, double y, double z) noexcept : x(x), y(y), z(z) {}
    
    // Copy constructor
    constexpr Vector3D(const Vector3D& other) noexcept = default;
    
    // Move constructor
    constexpr Vector3D(Vector3D&& other) noexcept = default;
    
    // Assignment operators
    constexpr Vector3D& operator=(const Vector3D& other) noexcept = default;
    constexpr Vector3D& operator=(Vector3D&& other) noexcept = default;
    
    // Vector addition
    constexpr inline Vector3D operator+(const Vector3D& rhs) const noexcept {
        return Vector3D(x + rhs.x, y + rhs.y, z + rhs.z);
    }
    
    // Vector subtraction
    constexpr inline Vector3D operator-(const Vector3D& rhs) const noexcept {
        return Vector3D(x - rhs.x, y - rhs.y, z - rhs.z);
    }
    
    // Scalar multiplication
    constexpr inline Vector3D operator*(double scalar) const noexcept {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }
    
    // Scalar division
    inline Vector3D operator/(double scalar) const {
        double invScalar = 1.0 / scalar; // Compute once instead of three divisions
        return Vector3D(x * invScalar, y * invScalar, z * invScalar);
    }
    
    // Compound assignment operators
    constexpr inline Vector3D& operator+=(const Vector3D& rhs) noexcept {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
    
    constexpr inline Vector3D& operator-=(const Vector3D& rhs) noexcept {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }
    
    constexpr inline Vector3D& operator*=(double scalar) noexcept {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    
    inline Vector3D& operator/=(double scalar) {
        double invScalar = 1.0 / scalar; // Compute once instead of three divisions
        x *= invScalar;
        y *= invScalar;
        z *= invScalar;
        return *this;
    }
    
    // Dot product
    constexpr inline double dot(const Vector3D& rhs) const noexcept {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }
    
    // Cross product
    constexpr inline Vector3D cross(const Vector3D& rhs) const noexcept {
        return Vector3D(
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        );
    }
    
    // Magnitude (length) of the vector
    inline double magnitude() const noexcept {
        return std::sqrt(magnitudeSquared());
    }
    
    // Squared magnitude (faster when only comparing lengths)
    constexpr inline double magnitudeSquared() const noexcept {
        return x * x + y * y + z * z;
    }
    
    // Normalize the vector (make it unit length)
    inline Vector3D normalize() const {
        double mag = magnitude();
        if (mag > 0) {
            double invMag = 1.0 / mag;
            return Vector3D(x * invMag, y * invMag, z * invMag);
        }
        return Vector3D();
    }
    
    // Check if vector is zero (within epsilon)
    constexpr inline bool isZero(double epsilon = 1e-10) const noexcept {
        return magnitudeSquared() < epsilon * epsilon;
    }
    
    // Distance between two points
    static inline double distance(const Vector3D& a, const Vector3D& b) noexcept {
        return (b - a).magnitude();
    }
    
    // Squared distance between two points (faster)
    static constexpr inline double distanceSquared(const Vector3D& a, const Vector3D& b) noexcept {
        return (b - a).magnitudeSquared();
    }
    
    // Linear interpolation between two vectors
    static inline Vector3D lerp(const Vector3D& a, const Vector3D& b, double t) noexcept {
        return a + (b - a) * t;
    }
};

// Stream output operator
inline std::ostream& operator<<(std::ostream& os, const Vector3D& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

// Scalar multiplication (scalar * vector)
constexpr inline Vector3D operator*(double scalar, const Vector3D& v) noexcept {
    return v * scalar;
} 