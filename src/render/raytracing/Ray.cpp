#include "Ray.h"

Ray::Ray(const sf::Vector3f& orig, const sf::Vector3f& dir) {
    m_orig = orig;
    m_dir = dir;
}

Ray& Ray::operator=(const Ray& copyRay) {
    if (this == &copyRay) return *this;
    m_orig = copyRay.m_orig;
    m_dir = copyRay.m_dir;
    return *this;
}

sf::Vector3f Ray::At(const float& t) const {
    return m_orig + (m_dir * t);
}
