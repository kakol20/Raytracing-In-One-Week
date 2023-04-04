#pragma once

#include "../maths/Vector3D.h"

namespace ColorTools {
	/// <summary>
	/// Converts Kelvin Temperature to RGB color
	/// </summary>
	/// <param name="k"></param>
	/// <returns></returns>
	Vector3D KelvinToRGB(const Float k);
}