#pragma once

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

  vec3(f32 e0, f32 e1, f32 e2) {
    e[0] = e0;
    e[1] = e1;
    e[2] = e2;
  }

  inline const vec3& operator+() { return *this; }
  inline vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
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

inline std::istream& operator>>(std::istream& is, vec3& v) {
  is >> v.e[0] >> v.e[1] >> v.e[2];
  return is;
}

inline std::ostream& operator<<(std::ostream& os, vec3& v) {
  os << "(" << v.e[0] << ", " << v.e[1] << ", " << v.e[2] << ")";
  return os;
}

inline vec3 operator+(const vec3& v1, const vec3& v2) {
  return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline vec3 operator-(const vec3& v1, const vec3& v2) {
  return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline vec3 operator*(const vec3& v1, const vec3& v2) {
  return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

inline vec3 operator/(const vec3& v1, const vec3& v2) {
  return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

inline vec3 operator*(const f32 t, const vec3& v) {
  return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, const f32 t) {
  return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator/(const vec3& v, const f32 t) {
  return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

inline vec3 normalize(const vec3& v) {
  f32 k = 1.0f / v.length();
  return vec3(v.e[0] * k, v.e[1] * k, v.e[2] * k);
}

inline f32 dot(const vec3& v1, const vec3& v2) {
  return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

inline vec3 cross(const vec3& v1, const vec3& v2) {
  return vec3(v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1],
              -(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0]),
              v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]);
}

inline vec3 lerp(const vec3& v1, const vec3& v2, f32 t) {
  return (1 - t) * v1 + t * v2;
}

inline f32 min(f32 t, f32 min) {
  return t < min ? min : t;
}

inline f32 max(f32 t, f32 max) {
  return t > max ? max : t;
}

inline f32 clamp(f32 t) {
  return min(max(t, 1), 0);
}

vec3 randomPointInUnitSphere() {
  vec3 result;

  do {
    result = 2.0f * vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
  } while (result.length2() >= 1.0f);

  return result;
}

vec3 randomPointInUnitDisk() {
  vec3 result;

  do {
    result = 2.0f * vec3(drand48(), drand48(), 0) - vec3(1, 1, 0);
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