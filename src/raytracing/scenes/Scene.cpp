#include "Scene.h"

Vector3D Scene::RayColor(Ray& ray, const int depth, const Vector3D& initialRayCol) {
	if (depth <= 0) return Vector3D(Float::NearZero);

	std::string test = (*m_settings)["renderMode"];

	// check for object hit
	HitRec rec;
	if (RayHitObject(ray, m_clipStart, m_clipEnd, rec)) {
		Ray scattered;
		Vector3D attentuation;
		Vector3D normal;
		bool absorb, transparent, emission;

		rec.GetMat()->Scatter(ray, rec, attentuation, scattered, normal, absorb, transparent, emission);

		attentuation *= initialRayCol;

		if ((*m_settings)["renderMode"] == "color") {
			if (absorb || emission) {
				return attentuation;
			} else if (transparent) {
				Ray continueRay(rec.GetPoint(), ray.GetDir());
				return RayColor(continueRay, depth - 1, initialRayCol);
			} else {
				//return attentuation * RayColor(scattered, depth - 1);
				//if (attentuation.NearZero()) return attentuation;
				return attentuation * RayColor(scattered, depth - 1, initialRayCol);
			}
		} else if ((*m_settings)["renderMode"] == "normal") {
			if (transparent) {
				Ray continueRay(rec.GetPoint(), ray.GetDir());
				return RayColor(continueRay, depth - 1, initialRayCol);
			} else {
				return ((m_camera.OutputVector(normal) + Vector3D::One) / 2) * initialRayCol;
			}
		} else if ((*m_settings)["renderMode"] == "albedo") {
			return attentuation;
		} else if ((*m_settings)["renderMode"] == "emission") {
			if (emission) {
				return attentuation;
			} else {
				return Vector3D::Zero;
			}
		}
	}

	// draw background instead if no hit

	Vector3D unitDir = ray.GetDir();
	unitDir.Normalize();
	if ((*m_settings)["renderMode"] == "color" || (*m_settings)["renderMode"] == "albedo") {
		//if ((*m_settings)["scene"] == "original") {
		//	Float t = 0.5 * (unitDir.GetY() + 1);
		//	return ((Vector3D::One * (1 - t)) + (Vector3D(0.5, 0.7, 1) * t)) * initialRayCol;
		//} else {
		//	Vector3D uv = unitDir.UVSphere();
		//	uv *= Vector3D(-1, 1, 1);

		//	// for debugging purposes
		//	//uv *= 10;

		//	Float u = uv.GetX() * m_background.GetWidth();
		//	Float v = uv.GetY() * m_background.GetHeight();

		//	Float r, g, b;
		//	m_background.GetColor(u, v, r, g, b);

		//	Vector3D rgb(r, g, b);
		//	rgb /= 255;
		//	rgb *= initialRayCol;

		//	return rgb * m_bgStrength;
		//}

		return BackgroundRay(unitDir);
	} else if ((*m_settings)["renderMode"] == "normal") {
		return ((m_camera.OutputVector(unitDir * -1) + Vector3D::One) / 2) * initialRayCol;
	} else if ((*m_settings)["renderMode"] == "emission") {
		return Vector3D::Zero;
	}

	return Vector3D::Zero;
}

bool Scene::RayHitObject(Ray& ray, const Float t_min, const Float t_max, HitRec& rec) {
	HitRec tempRec;
	bool hit = false;
	Float closest = t_max;

	for (auto it = m_renderedObjects.begin(); it != m_renderedObjects.end(); it++) {
		if ((*it)->Hit(ray, t_min, closest, tempRec)) {
			hit = true;
			closest = tempRec.GetT();
			rec = tempRec;
		}
	}

	return hit;
}
