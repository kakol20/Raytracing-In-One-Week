#pragma once
#include "Vector3D.h"

class ColorSpace {
public:
	ColorSpace() {};
	~ColorSpace() {};
	
	/// <summary>
	/// Convert Linear RGB to CIE XYZ
	/// </summary>
	/// <param name="linear">Between 0 and 1</param>
	/// <returns></returns>
	static Vector3D LinearToCIEXYZ(const Vector3D linear);

	/// <summary>
	/// Convert CIE XYZ to ACES2065-1
	/// </summary>
	/// <param name="xyz">Between 0 and 1</param>
	/// <returns></returns>
	static Vector3D CIEXYZToACES(const Vector3D xyz);

	/// <summary>
	/// Convert Linear RGB to sRGB
	/// </summary>
	/// <param name="linear">Between 0 and 1</param>
	/// <returns></returns>
	static Vector3D LinearTosRGB(const Vector3D linear);
};

