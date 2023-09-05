#include "ACES.h"

// https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl

Vector3D ACES::ToneMap(const Vector3D& col) {
	Vector3D out = ACES::InputMat(col);
	out = ACES::RRTAndODTFit(out);
	out = ACES::OutputMat(out);

	return Vector3D::Clamp(out, Vector3D::Zero, Vector3D::One);

	//return col;
}

Vector3D ACES::InputMat(const Vector3D& v) {
	// https://www.gamedev.net/forums/topic/268090-hlsl-float3x3-constructor/2648805/
	Vector3D x_m(0.59719, 0.35458, 0.04823);
	Float x = Vector3D::DotProduct(x_m, v);

	Vector3D y_m(0.07600, 0.90834, 0.01566);
	Float y = Vector3D::DotProduct(y_m, v);

	Vector3D z_m(0.02840, 0.13383, 0.83777);
	Float z = Vector3D::DotProduct(z_m, v);

	return Vector3D(x, y, z);
}

Vector3D ACES::RRTAndODTFit(const Vector3D& v) {
	Vector3D a = v * (v + Vector3D(0.0245786)) - Vector3D(0.000090537);
	Vector3D b = v * (Vector3D(0.983729) * v + Vector3D(0.4329510)) + Vector3D(0.238081);
	return a / b;
}

Vector3D ACES::OutputMat(const Vector3D& v) {
	Vector3D x_m(1.60475, -0.53108, -0.07367);
	Float x = Vector3D::DotProduct(x_m, v);

	Vector3D y_m(-0.10208, 1.10813, -0.00605);
	Float y = Vector3D::DotProduct(y_m, v);

	Vector3D z_m(-0.00327, -0.07276, 1.07602);
	Float z = Vector3D::DotProduct(z_m, v);

	return Vector3D(x, y, z);
}
