#include <cmath>

#include "Matrix3x3.h"

#include "ColorSpace.h"

Vector3D ColorSpace::LinearToCIEXYZ(const Vector3D linear) {
    Matrix3x3 mat(Vector3D(0.4124f, 0.3576f, 0.1805f), 
        Vector3D(0.2126f, 0.7152f, 0.0722f), 
        Vector3D(0.0193f, 0.1192f, 0.9505f));

    return mat * linear;;
}

Vector3D ColorSpace::CIEXYZToACES(const Vector3D xyz) {
    Matrix3x3 mat(Vector3D(1.0498110175f, 0.f, -0.0000974845f), 
        Vector3D(-0.4959030231f, 1.3733130458f, 0.0982400361f), 
        Vector3D(0.f, 0.f, 0.9912520182f));

    return mat * xyz;
}

Vector3D ColorSpace::LinearTosRGB(const Vector3D linear) {
    Vector3D l_linear = linear;
    float r, g, b;
    for (int i = 0; i < 3; i++) {
        float temp;
        if (i == 0) {
            temp = l_linear.GetX();
        }
        else if (i == 1) {
            temp = l_linear.GetY();
        }
        else {
            temp = l_linear.GetZ();
        }

        if (temp <= 0.0031308f) {
            temp *= 12.92f;
        }
        else {
            temp = (1.055f * pow(temp, 1.f / 2.4f)) - 0.055f;
        }

        if (i == 0) {
            r = temp;
        }
        else if (i == 1) {
            g = temp;
        }
        else {
            b = temp;
        }
    }
    return Vector3D(r, g, b);
}
