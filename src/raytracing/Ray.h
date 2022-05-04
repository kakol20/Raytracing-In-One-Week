#ifndef RAY_H
#define RAY_H

#include "../maths/Vector3D.h"

class Ray {
public:
	Ray() : Ray(Vector3D(0), Vector3D::Forward) {};
	Ray(const Vector3D& orig, const Vector3D& dir);
	~Ray() {};

	Ray& operator=(const Ray& copyRay);

	Vector3D At(const Float& t) const;

	Vector3D GetDir() const { return m_dir; };
	void SetDir(const Vector3D& dir) { m_dir = dir; };

	Vector3D GetOrig() const { return m_orig; };
	void SetOrig(const Vector3D orig) { m_orig = orig; };

private:
	Vector3D m_dir;
	Vector3D m_orig;
};

#endif // !RAY_H