#pragma once

namespace math {

//
// vec3
//

struct vec3 {
  union {
    struct {
      f32 x, y, z;
    };
    struct {
      f32 r, g, b;
    };
    f32 e[3];
  };

  vec3() {}
  vec3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}

  inline const vec3& operator+() { return *this; }
  inline vec3 operator-() const { return {-e[0], -e[1], -e[2]}; }
  inline f32 operator[](u32 i) const { return e[i]; }
  inline f32& operator[](u32 i) { return e[i]; }

  inline vec3& operator+=(const vec3& v2) {
    this->e[0] += v2.e[0];
    this->e[1] += v2.e[1];
    this->e[2] += v2.e[2];
    return *this;
  }
  inline vec3& operator-=(const vec3& v2) {
    this->e[0] -= v2.e[0];
    this->e[1] -= v2.e[1];
    this->e[2] -= v2.e[2];
    return *this;
  }

  inline vec3& operator*=(const vec3& v2) {
    this->e[0] *= v2.e[0];
    this->e[1] *= v2.e[1];
    this->e[2] *= v2.e[2];
    return *this;
  }

  inline vec3& operator/=(const vec3& v2) {
    this->e[0] /= v2.e[0];
    this->e[1] /= v2.e[1];
    this->e[2] /= v2.e[2];
    return *this;
  }

  inline vec3& operator*=(const f32 t) {
    this->e[0] *= t;
    this->e[1] *= t;
    this->e[2] *= t;
    return *this;
  }

  inline vec3& operator/=(const f32 t) {
    this->e[0] /= t;
    this->e[1] /= t;
    this->e[2] /= t;
    return *this;
  }

  inline f32 length() const {
    return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
  }

  inline f32 length2() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
};

inline std::ostream& operator<<(std::ostream& os, const vec3& v) {
  os << "(" << v.e[0] << ", " << v.e[1] << ", " << v.e[2] << ")";
  return os;
}

inline vec3 operator+(const vec3& v1, const vec3& v2) {
  return {v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]};
}

inline vec3 operator-(const vec3& v1, const vec3& v2) {
  return {v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]};
}

inline vec3 operator*(const vec3& v1, const vec3& v2) {
  return {v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]};
}

inline vec3 operator/(const vec3& v1, const vec3& v2) {
  return {v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]};
}

inline vec3 operator*(const f32 t, const vec3& v) {
  return {t * v.e[0], t * v.e[1], t * v.e[2]};
}

inline vec3 operator*(const vec3& v, const f32 t) {
  return {t * v.e[0], t * v.e[1], t * v.e[2]};
}

inline vec3 operator/(const vec3& v, const f32 t) {
  return {v.e[0] / t, v.e[1] / t, v.e[2] / t};
}

inline vec3 normalize(const vec3& v) {
  f32 k = 1.0f / v.length();
  return {v.e[0] * k, v.e[1] * k, v.e[2] * k};
}

inline f32 dot(const vec3& v1, const vec3& v2) {
  return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

inline vec3 cross(const vec3& v1, const vec3& v2) {
  return {v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1],
          -(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0]),
          v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]};
}

inline vec3 lerp(const vec3& v1, const vec3& v2, f32 t) {
  return (1 - t) * v1 + t * v2;
}

//
// vec4
//

struct vec4 {
  union {
    struct {
      f32 x, y, z, w;
    };
    struct {
      f32 r, g, b, a;
    };
    f32 e[4];
  };

  vec4() {}
  vec4(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w) {}

  inline const vec4& operator+() { return *this; }
  inline vec4 operator-() const { return {-e[0], -e[1], -e[2], -e[3]}; }
  inline f32 operator[](u32 i) const { return e[i]; }
  inline f32& operator[](u32 i) { return e[i]; }

  inline vec4& operator+=(const vec4& v2) {
    this->e[0] += v2.e[0];
    this->e[1] += v2.e[1];
    this->e[2] += v2.e[2];
    this->e[3] += v2.e[3];
    return *this;
  }
  inline vec4& operator-=(const vec4& v2) {
    this->e[0] -= v2.e[0];
    this->e[1] -= v2.e[1];
    this->e[2] -= v2.e[2];
    this->e[3] -= v2.e[3];
    return *this;
  }

  inline vec4& operator*=(const vec4& v2) {
    this->e[0] *= v2.e[0];
    this->e[1] *= v2.e[1];
    this->e[2] *= v2.e[2];
    this->e[3] *= v2.e[3];
    return *this;
  }

  inline vec4& operator/=(const vec4& v2) {
    this->e[0] /= v2.e[0];
    this->e[1] /= v2.e[1];
    this->e[2] /= v2.e[2];
    this->e[3] /= v2.e[3];
    return *this;
  }

  inline vec4& operator*=(const f32 t) {
    this->e[0] *= t;
    this->e[1] *= t;
    this->e[2] *= t;
    this->e[3] *= t;
    return *this;
  }

  inline vec4& operator/=(const f32 t) {
    this->e[0] /= t;
    this->e[1] /= t;
    this->e[2] /= t;
    this->e[3] /= t;
    return *this;
  }
};

inline std::ostream& operator<<(std::ostream& os, const vec4& v) {
  os << "(" << v.e[0] << ", " << v.e[1] << ", " << v.e[2] << ", " << v.e[3]
     << ")";
  return os;
}

inline vec4 operator+(const vec4& v1, const vec4& v2) {
  return {v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2],
          v1.e[3] + v2.e[3]};
}

inline vec4 operator-(const vec4& v1, const vec4& v2) {
  return {v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2],
          v1.e[3] - v2.e[3]};
}

inline vec4 operator*(const vec4& v1, const vec4& v2) {
  return {v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2],
          v1.e[3] * v2.e[3]};
}

inline vec4 operator/(const vec4& v1, const vec4& v2) {
  return {v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2],
          v1.e[3] / v2.e[3]};
}

inline vec4 operator*(const f32 t, const vec4& v) {
  return {t * v.e[0], t * v.e[1], t * v.e[2], t * v.e[3]};
}

inline vec4 operator*(const vec4& v, const f32 t) {
  return {t * v.e[0], t * v.e[1], t * v.e[2], t * v.e[3]};
}

inline vec4 operator/(const vec4& v, const f32 t) {
  return {v.e[0] / t, v.e[1] / t, v.e[2] / t, v.e[3] / t};
}

//
// ivec3
//

struct ivec3 {
  union {
    struct {
      u32 x, y, z;
    };
    struct {
      u32 r, g, b;
    };
    u32 e[3];
  };

  ivec3() {}
  ivec3(u32 x, u32 y, u32 z) : x(x), y(y), z(z) {}

  inline const ivec3& operator+() { return *this; }
  inline ivec3 operator-() const { return {-e[0], -e[1], -e[2]}; }
  inline u32 operator[](u32 i) const { return e[i]; }
  inline u32& operator[](u32 i) { return e[i]; }
};

//
// mat4
//

struct mat4 {
  union {
    f32 e[16];
  };
};

inline mat4 orthographic(f32 left,
                         f32 right,
                         f32 top,
                         f32 bottom,
                         f32 near,
                         f32 far) {
  mat4 result = {2.0f / (right - left),
                 0,
                 0,
                 0,
                 0,
                 2.0f / (top - bottom),
                 0,
                 0,
                 0,
                 0,
                 -2.0f / (far - near),
                 0,
                 -((right + left) / (right - left)),
                 -((top + bottom) / (top - bottom)),
                 -((far + near) / (far - near)),
                 1};
  return result;
}

//
// Other
//

inline f32 min(f32 a, f32 b) {
  return a < b ? a : b;
}

inline f32 max(f32 a, f32 b) {
  return a > b ? a : b;
}

inline f32 clamp(f32 t) {
  return max(min(t, 1), 0);
}

inline f32 clamp(f32 t, f32 _min, f32 _max) {
  return max(min(t, _max), _min);
}

inline f32 rand01() {
  return f32(drand48());
}

inline f32 radians(f32 degrees) {
  return degrees * M_PI / 180.0f;
}

vec3 randomPointInUnitSphere() {
  vec3 result;

  do {
    result = 2.0f * vec3{rand01(), rand01(), rand01()} - vec3{1, 1, 1};
  } while (result.length2() >= 1.0f);

  return result;
}

vec3 randomPointInUnitDisk() {
  vec3 result;

  do {
    result = 2.0f * vec3{rand01(), rand01(), 0} - vec3{1, 1, 0};
  } while (result.length2() >= 1.0f);

  return result;
}

vec3 reflect(const vec3& v, const vec3& normal) {
  return v - 2 * dot(v, normal) * normal;
}

bool refract(const vec3& v,
             const vec3& normal,
             const f32 refractionRatio,
             vec3& refracted) {
  vec3 uv = normalize(v);
  float dt = dot(uv, normal);
  float discriminant = 1 - refractionRatio * refractionRatio * (1 - dt * dt);

  if (discriminant > 0) {
    refracted =
        refractionRatio * (uv - normal * dt) - normal * sqrt(discriminant);
    return true;
  } else {
    return false;
  }
}

f32 schlick(f32 cosine, f32 refractiveIndex) {
  f32 r0 = (1 - refractiveIndex) / (1 + refractiveIndex);
  r0 *= r0;
  return r0 + (1 - r0) * pow(1 - cosine, 5);
}

//
// Ray
//

struct Ray {
  math::vec3 origin;
  math::vec3 direction;

  inline math::vec3 at(const f32 t) const { return origin + t * direction; }
};

//
// AABB
//

struct AABB {
  math::vec3 minPoint;
  math::vec3 maxPoint;

  AABB() : minPoint({0, 0, 0}), maxPoint({0, 0, 0}) {}

  AABB(math::vec3 minPoint, math::vec3 maxPoint)
      : minPoint(minPoint), maxPoint(maxPoint) {}

  bool hit(const math::Ray& ray, f32 tMin, f32 tMax) const;
};

// TODO: Upgrade to Andrew Kensler's faster code
bool AABB::hit(const math::Ray& ray, f32 tMin, f32 tMax) const {
  for (u32 axis = 0; axis < 3; axis++) {
    f32 t0 =
        math::min((minPoint[axis] - ray.origin[axis]) / ray.direction[axis],
                  (maxPoint[axis] - ray.origin[axis]) / ray.direction[axis]);
    f32 t1 =
        math::max((minPoint[axis] - ray.origin[axis]) / ray.direction[axis],
                  (maxPoint[axis] - ray.origin[axis]) / ray.direction[axis]);

    tMin = math::max(t0, tMin);
    tMax = math::min(t1, tMax);

    if (tMax <= tMin)
      return false;
  }
  return true;
}

AABB surround(const AABB& box0, const AABB& box1) {
  math::vec3 minPoint{math::min(box0.minPoint.x, box1.minPoint.x),
                      math::min(box0.minPoint.y, box1.minPoint.y),
                      math::min(box0.minPoint.z, box1.minPoint.z)};
  math::vec3 maxPoint{math::max(box0.maxPoint.x, box1.maxPoint.x),
                      math::max(box0.maxPoint.y, box1.maxPoint.y),
                      math::max(box0.maxPoint.z, box1.maxPoint.z)};
  return AABB(minPoint, maxPoint);
}

}  // namespace math