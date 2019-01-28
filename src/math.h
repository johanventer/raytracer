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
  os << "[" << v.e[0] << ", " << v.e[1] << ", " << v.e[2] << "]";
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

static std::mt19937 randomGenerator;
static std::uniform_real_distribution<f32> randomDistribution;
auto rand01 = std::bind(randomDistribution, randomGenerator);

inline f32 radians(f32 degrees) {
  return degrees * M_PI / 180.0f;
}

inline vec3 randomPointInUnitSphere() {
  vec3 result;

  do {
    result = 2.0f * vec3{rand01(), rand01(), rand01()} - vec3{1, 1, 1};
  } while (result.length2() >= 1.0f);

  return result;
}

inline vec3 randomPointInUnitDisk() {
  vec3 result;

  do {
    result = 2.0f * vec3{rand01(), rand01(), 0} - vec3{1, 1, 0};
  } while (result.length2() >= 1.0f);

  return result;
}

inline vec3 reflect(const vec3& v, const vec3& normal) {
  return v - 2 * dot(v, normal) * normal;
}

inline bool refract(const vec3& v,
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

inline f32 schlick(f32 cosine, f32 refractiveIndex) {
  f32 r0 = (1 - refractiveIndex) / (1 + refractiveIndex);
  r0 *= r0;
  return r0 + (1 - r0) * pow(1 - cosine, 5);
}

inline void sphereTextureCoordinates(vec3 p, f32& u, f32& v) {
  f32 phi = atan2(p.z, p.x);
  f32 theta = asin(p.y);
  u = 1 - (phi + M_PI) / (2 * M_PI);
  v = (theta + M_PI / 2) / M_PI;
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

bool AABB::hit(const math::Ray& ray, f32 tMin, f32 tMax) const {
  for (u32 axis = 0; axis < 3; axis++) {
    f32 invD = 1.0f / ray.direction[axis];

    f32 t0 = math::min((minPoint[axis] - ray.origin[axis]) * invD,
                       (maxPoint[axis] - ray.origin[axis]) * invD);
    f32 t1 = math::max((minPoint[axis] - ray.origin[axis]) * invD,
                       (maxPoint[axis] - ray.origin[axis]) * invD);

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

//
// Perlin Noise
//

/**
 * Generates random vectors in the range [-1,1] and normalises them.
 */
#if 0
vec3* perlinGenerateVectors() {
  vec3* randomVectors = new vec3[256];
  for (u32 i = 0; i < 256; ++i) {
    randomVectors[i] = normalize(
        vec3(-1 + 2 * rand01(), -1 + 2 * rand01(), -1 + 2 * rand01()));
  }
  return randomVectors;
}
#else
// NOTE(johan): Possibly better uniform gradients? But I can't really tell the
// difference.
vec3* perlinGenerateVectors() {
  vec3* randomVectors = new vec3[256];
  for (u32 i = 0; i < 256; ++i) {
    f32 theta = acos(2 * rand01() - 1);
    f32 phi = 2 * rand01() * M_PI;

    randomVectors[i] = normalize(
        vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta)));
  }
  return randomVectors;
}
#endif

/**
 * Generates a shuffled array of unsigned integers in the range [0, 255].
 */
u32* perlinGeneratePermutations() {
  u32* permutations = new u32[256];

  for (u32 i = 0; i < 256; i++) {
    permutations[i] = i;
  }

  // Randomly shuffles the numbers in the array
  for (u32 i = 255 - 1; i > 0; i--) {
    u32 target = u32(rand01() * (i + 1));
    std::swap(permutations[i], permutations[target]);
  }

  return permutations;
}

/**
 * Smoothstep t-value remapping.
 * "Hermite cubic"
 */
inline f32 smoothstep(const f32& t) {
  return t * t * (3 - 2 * t);
}

inline f32 perlinInterpolate(vec3 grid[2][2][2], f32 tx, f32 ty, f32 tz) {
  f32 u = smoothstep(tx);
  f32 v = smoothstep(ty);
  f32 w = smoothstep(tz);

  f32 accum = 0;

  for (u32 i = 0; i < 2; i++)
    for (u32 j = 0; j < 2; j++)
      for (u32 k = 0; k < 2; k++) {
        vec3 weight(tx - i, ty - j, tz - k);
        accum += (i * u + (1 - i) * (1 - u)) * (j * v + (1 - j) * (1 - v)) *
                 (k * w + (1 - k) * (1 - w)) * dot(grid[i][j][k], weight);
      }

  return accum;
}

struct Perlin {
  static vec3* gradients;
  static u32* xPermute;
  static u32* yPermute;
  static u32* zPermute;

  f32 noise(const vec3& p) const {
    // The min integer indices into the grid
    s32 i = floor(p.x);
    s32 j = floor(p.y);
    s32 k = floor(p.z);

    // The t-values between the grid points for interpolation
    f32 tx = p.x - i;
    f32 ty = p.y - j;
    f32 tz = p.z - k;

    vec3 grid[2][2][2];

    // Fetch the random lattice grid points that surround the position
    for (u32 di = 0; di < 2; di++)
      for (u32 dj = 0; dj < 2; dj++)
        for (u32 dk = 0; dk < 2; dk++)
          grid[di][dj][dk] =
              gradients[xPermute[(i + di) & 255] ^ yPermute[(j + dj) & 255] ^
                        zPermute[(k + dk) & 255]];

    // Interpolate!
    return perlinInterpolate(grid, tx, ty, tz);
  }

  /**
   * Adds together multiple noise functions, halving the amplitude and doubling
   * the frequency for each curve.
   */
  f32 turbulence(const vec3& p,
                 f32 amplitude = 1,
                 f32 frequency = 1,
                 f32 amplitudeMultiplier = 0.5,
                 f32 frequencyMultiplier = 2,
                 vec3 offset = {0, 0, 0},
                 s32 depth = 7) const {
    f32 result = 0;
    for (u32 i = 0; i < depth; i++) {
      result += amplitude * noise(frequency * p + offset);
      amplitude *= amplitudeMultiplier;
      frequency *= frequencyMultiplier;
    }
    return fabs(result);
  }
};

vec3* Perlin::gradients = perlinGenerateVectors();
u32* Perlin::xPermute = perlinGeneratePermutations();
u32* Perlin::yPermute = perlinGeneratePermutations();
u32* Perlin::zPermute = perlinGeneratePermutations();

}  // namespace math