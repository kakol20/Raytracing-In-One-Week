#pragma once

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

#include "../../maths/Vector3.h"
#include "Ray.h"

class Material;

class HitRec {
public:
	HitRec();
	HitRec(const HitRec& copyHitRec);
	~HitRec();

	HitRec& operator=(const HitRec& copyHitRec);

	void SetFaceNormal(Ray& ray, sf::Vector3f& outwardNormal);

	const bool GetFrontFace() const { return m_frontFace; };
	void SetFrontFace(const bool flag) { m_frontFace = flag; };

	const float GetT() const { return m_t; };
	void SetT(const float& t) { m_t = t; };

	Material* GetMat() { return m_mat; };
	void SetMat(Material* mat) { m_mat = mat; };

	const sf::Vector3f GetNormal() const { return m_normal; };
	void SetNormal(const sf::Vector3f& normal) {
		m_normal = normal;
		m_normal = rt::Vector3::Normalize(m_normal);
	};

	const sf::Vector3f GetPoint() const { return m_point; };
	void SetPoint(const sf::Vector3f& point) { m_point = point; };

	const sf::Vector3f GetUV() const { return m_uv; };
	void SetUV(const sf::Vector3f& uv) { m_uv = uv; };

	const sf::Vector3f GetTangent() const { return m_tangent; };
	void SetTangent(const sf::Vector3f& tangent) {
		m_tangent = tangent;
		m_tangent = rt::Vector3::Normalize(m_tangent);
	};

	const sf::Vector3f GetBitangent() const { return m_bitangent; };
	void SetBitangent(const sf::Vector3f& bitangent) {
		m_bitangent = bitangent;
		m_bitangent = rt::Vector3::Normalize(m_bitangent);
	};

	const sf::Vector3f TangentToWorld(const sf::Vector3f& tangentSpace) const;

private:
	bool m_frontFace;
	float m_t;
	Material* m_mat;
	sf::Vector3f m_bitangent;
	sf::Vector3f m_normal;
	sf::Vector3f m_point;
	sf::Vector3f m_tangent;
	sf::Vector3f m_uv;
};