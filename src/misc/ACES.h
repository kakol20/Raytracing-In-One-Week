#pragma once

#include "../maths/Vector3D.h"

class ACES {
public:
	ACES() {};
	~ACES() {};

	static Vector3D ToneMap(const Vector3D& col);

private:
	static Vector3D InputMat(const Vector3D& v);

	static Vector3D RRTAndODTFit(const Vector3D& v);

	static Vector3D OutputMat(const Vector3D& v);
};