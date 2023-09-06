#pragma once

#include "../maths/Vector3D.h"

namespace ColorTools {
	/// <summary>
	/// Converts Kelvin Temperature to RGB color
	/// </summary>
	/// <param name="k"></param>
	/// <returns></returns>
	Vector3D KelvinToRGB(const Float k);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="H">0 to 360</param>
	/// <param name="S">0 to 1</param>
	/// <param name="V">0 to 1</param>
	/// <returns></returns>
	Vector3D HSVToRGB(Float H, Float S, Float V);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="col"></param>
	/// <param name="H">0 to 360</param>
	/// <param name="S">0 to 1</param>
	/// <param name="V">0 to 1</param>
	void RGBtoHSV(const Vector3D& col, Float& H, Float& S, Float& V);
}