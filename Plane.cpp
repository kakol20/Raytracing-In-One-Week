#include <algorithm>
#include <math.h>

#include "Plane.h"

Plane::Plane(Plane::Type type, const Vector3D origin, const float width, const float height, Material* mat, const Vector3D uvScale) {
	m_height = height;
	m_mat = mat;
	m_pos = origin;
	m_type = type;
	m_uvScale = uvScale;
	m_width = width;
}

Plane::~Plane() {
	m_mat = nullptr;
}

bool Plane::Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	Vector3D n;
	switch (m_type) {
	case Plane::Type::XPlus:
		n = Vector3D(1.f, 0.f, 0.f);
		break;
	case Plane::Type::XMinus:
		n = Vector3D(-1.f, 0.f, 0.f);
		break;
	case Plane::Type::YPlus:
		n = Vector3D(0.f, 1.f, 0.f);
		break;
	case Plane::Type::YMinus:
		n = Vector3D(0.f, -1.f, 0.f);
		break;
	case Plane::Type::ZPlus:
		n = Vector3D(0.f, 0.f, 1.f);
		break;
	case Plane::Type::ZMinus:
		n = Vector3D(0.f, 0.f, -1.f);
		break;
	default:
		return false;
	}

	n.Normalize();

	Vector3D l = ray.GetDir();
	//l.Normalize();
	float dotLN = Vector3D::DotProduct(l, n);
	if (dotLN != 0.f) {
		Vector3D p0 = m_pos;
		Vector3D l0 = ray.GetOrig();

		float d = Vector3D::DotProduct(p0 - l0, n) / dotLN;
		
		d = fabsf(d);
		if (t_min > d || d > t_max) return false;

		Vector3D p = l0 + (l * d);
		float nearZero = 1e-4f;

		Vector3D divide(fminf(m_height, m_width));

		if (m_type == Plane::Type::XMinus || m_type == Plane::Type::XPlus) {
			Vector3D offset = Vector3D(0.f, m_height / 2.f, m_width / 2.f);
			Vector3D min = m_pos - offset;
			Vector3D max = m_pos + offset;

			Vector3D axisClose = m_pos - p;
			axisClose *= Vector3D(1.f, 0.f, 0.f);

			if (min.GetY() <= p.GetY() && p.GetY() <= max.GetY() && min.GetZ() <= p.GetZ() && p.GetZ() <= max.GetZ() && axisClose.Threshold(nearZero)) {
				rec.SetT(d);
				rec.SetPoint(p);
				rec.SetMat(m_mat);
				rec.SetFaceNormal(ray, n);

				Vector3D bottomLeft, topRight;
				if (m_type == Plane::Type::XPlus) {
					bottomLeft = Vector3D(max.GetZ(), min.GetY());
					topRight = Vector3D(min.GetZ(), max.GetY());
					rec.SetTangents(Vector3D(0.f, 0.f, -1.f));
				}
				else {
					bottomLeft = Vector3D(min.GetZ(), min.GetY());
					topRight = Vector3D(max.GetZ(), max.GetY());
					rec.SetTangents(Vector3D(0.f, 0.f, 1.f));
				}

				Vector3D uv = Vector3D(p.GetZ(), p.GetY());
				divide += Vector3D(0.f, 0.f, 1.f); // prevent divide by zero
				uv = (uv - bottomLeft) / divide;
				rec.SetUV(uv * m_uvScale);

				return true;
			}
		}
		else if (m_type == Plane::Type::YMinus || m_type == Plane::Type::YPlus) {
			Vector3D offset = Vector3D(m_width / 2.f, 0.f, m_height / 2.f);
			Vector3D min = m_pos - offset;
			Vector3D max = m_pos + offset;

			Vector3D axisClose = m_pos - p;
			axisClose *= Vector3D(0.f, 1.f, 0.f);

			if (min.GetX() <= p.GetX() && p.GetX() <= max.GetX() && min.GetZ() <= p.GetZ() && p.GetZ() <= max.GetZ() && axisClose.Threshold(nearZero)) {
				rec.SetT(d);
				rec.SetPoint(p);
				rec.SetMat(m_mat);
				rec.SetFaceNormal(ray, n);

				Vector3D bottomLeft, topRight;
				if (m_type == Plane::Type::ZPlus) {
					bottomLeft = Vector3D(min.GetX(), max.GetZ());
					topRight = Vector3D(max.GetX(), min.GetZ());
					rec.SetTangents(Vector3D(1.f, 0.f, 0.f));
				}
				else {
					bottomLeft = Vector3D(max.GetX(), min.GetZ());
					topRight = Vector3D(min.GetX(), max.GetZ());
					rec.SetTangents(Vector3D(-1.f, 0.f, 0.f));
				}

				Vector3D uv = Vector3D(p.GetX(), p.GetZ());
				divide += Vector3D(0.f, 0.f, 1.f); // prevent divide by zero
				uv = (uv - bottomLeft) / divide;
				rec.SetUV(uv * m_uvScale);

				return true;
			}
		}
		else {
			Vector3D offset = Vector3D(m_width / 2.f, m_height / 2.f, 0.f);
			Vector3D min = m_pos - offset;
			Vector3D max = m_pos + offset;

			Vector3D axisClose = m_pos - p;
			axisClose *= Vector3D(0.f, 0.f, 1.f);

			if (min.GetX() <= p.GetX() && p.GetX() <= max.GetX() && min.GetY() <= p.GetY() && p.GetY() <= max.GetY() && axisClose.Threshold(nearZero)) {
				rec.SetT(d);
				rec.SetPoint(p);
				rec.SetMat(m_mat);
				rec.SetFaceNormal(ray, n);

				Vector3D bottomLeft, topRight;
				if (m_type == Plane::Type::ZPlus) {
					bottomLeft = Vector3D(min.GetX(), min.GetY());
					topRight = Vector3D(max.GetX(), max.GetY());
					rec.SetTangents(Vector3D(1.f, 0.f, 0.f));
				}
				else {
					bottomLeft = Vector3D(max.GetX(), min.GetY());
					topRight = Vector3D(min.GetX(), max.GetY());
					rec.SetTangents(Vector3D(-1.f, 0.f, 0.f));
				}

				Vector3D uv = Vector3D(p.GetX(), p.GetY());
				divide += Vector3D(0.f, 0.f, 1.f); // prevent divide by zero
				uv = (uv - bottomLeft) / divide;
				rec.SetUV(uv * m_uvScale);

				return true;
			}
		}
	}
	return false;
}