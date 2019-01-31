#pragma once

namespace math {

//
// vec3
//

#if SSE
struct vec3 {
  union {
    __m128 v;
    struct {
      f32 x, y, z, w;
    };
    struct {
      f32 r, g, b, a;
    };
    f32 e[4];
  } _MM_ALIGN16;

  vec3() {}
  vec3(__m128 v) : v(v) {}
  vec3(f32 x, f32 y, f32 z) { v = _mm_set_ps(0, z, y, x); }
  vec3(f32* e) { v = _mm_set_ps(0, e[2], e[1], e[0]); }

  inline const vec3& operator+() { return *this; }
  inline vec3 operator-() const { return _mm_sub_ps(_mm_set1_ps(0.0), v); }

  inline f32 operator[](u32 i) const { return e[i]; }

  inline vec3 operator+(const vec3& v2) const { return _mm_add_ps(v, v2.v); }
  inline vec3 operator-(const vec3& v2) const { return _mm_sub_ps(v, v2.v); }
  inline vec3 operator*(const vec3& v2) const { return _mm_mul_ps(v, v2.v); }
  inline vec3 operator/(const vec3& v2) const { return _mm_div_ps(v, v2.v); }

  inline vec3& operator+=(const vec3& v2) {
    v = _mm_add_ps(v, v2.v);
    return *this;
  }
  inline vec3& operator-=(const vec3& v2) {
    v = _mm_sub_ps(v, v2.v);
    return *this;
  }
  inline vec3& operator*=(const vec3& v2) {
    v = _mm_mul_ps(v, v2.v);
    return *this;
  }
  inline vec3& operator/=(const vec3& v2) {
    v = _mm_div_ps(v, v2.v);
    return *this;
  }

  inline vec3 operator*(const f32 t) const {
    return _mm_mul_ps(_mm_set1_ps(t), v);
  }
  inline vec3 operator/(const f32 t) const {
    return _mm_div_ps(v, _mm_set1_ps(t));
  }

  inline vec3& operator*=(const f32 t) {
    v = _mm_mul_ps(v, _mm_set1_ps(t));
    return *this;
  }
  inline vec3& operator/=(const f32 t) {
    v = _mm_div_ps(v, _mm_set1_ps(t));
    return *this;
  }
};

inline vec3 operator*(const f32 t, const vec3& v) {
  return _mm_mul_ps(v.v, _mm_set1_ps(t));
}

inline std::ostream& operator<<(std::ostream& os, const vec3& v) {
  os << "[" << v.x << ", " << v.y << ", " << v.z << "]";
  return os;
}

inline f32 length(const vec3& v) {
  return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(v.v, v.v, 0x71)));
}

inline f32 length2(const vec3& v) {
  return _mm_cvtss_f32(_mm_dp_ps(v.v, v.v, 0x71));
}

inline vec3 normalize(const vec3& v) {
#if 1
  __m128 inverseNorm = _mm_rsqrt_ps(_mm_dp_ps(v.v, v.v, 0x77));
  return _mm_mul_ps(v.v, inverseNorm);
#else
  // NOTE(johan): Slower but more accurate
  __m128 norm = _mm_sqrt_ps(_mm_dp_ps(v.v, v.v, 0x7F));
  return vec3(_mm_div_ps(v.v, norm));
#endif
}

inline f32 dot(const vec3& v1, const vec3& v2) {
  float result;
  __m128 dp = _mm_dp_ps(v1.v, v2.v, 0x7F);
  _mm_store_ss(&result, dp);
  return result;
}

inline vec3 cross(const vec3& v1, const vec3& v2) {
  return vec3(_mm_sub_ps(
      _mm_mul_ps(_mm_shuffle_ps(v1.v, v1.v, _MM_SHUFFLE(3, 0, 2, 1)),
                 _mm_shuffle_ps(v2.v, v2.v, _MM_SHUFFLE(3, 1, 0, 2))),
      _mm_mul_ps(_mm_shuffle_ps(v1.v, v1.v, _MM_SHUFFLE(3, 1, 0, 2)),
                 _mm_shuffle_ps(v2.v, v2.v, _MM_SHUFFLE(3, 0, 2, 1)))));
}

inline vec3 lerp(const vec3& v1, const vec3& v2, f32 t) {
  return (1 - t) * v1 + t * v2;
}

#else

struct vec3 {
  union {
    struct {
      f32 x, y, z;
    };
    struct {
      f32 r, g, b;
    };
    f32 e[4];
  };

  vec3() {}
  vec3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}
  vec3(f32* e) : x(e[0]), y(e[1]), z(e[2]) {}

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

inline f32 length(const vec3& v) {
  return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

inline f32 length2(const vec3& v) {
  return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

inline vec3 normalize(const vec3& v) {
  f32 k = 1.0f / length(v);
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

#endif

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

inline std::ostream& operator<<(std::ostream& os, const vec4& v) {
  os << "(" << v.e[0] << ", " << v.e[1] << ", " << v.e[2] << ", " << v.e[3]
     << ")";
  return os;
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
// Random numbers
//

#if 0
static std::mt19937 randomGenerator;
static std::uniform_real_distribution<f32> randomDistribution;
auto rand01 = std::bind(randomDistribution, randomGenerator);
#else
typedef struct {
  uint64_t state;
  uint64_t inc;
} pcg32_random_t;

static pcg32_random_t pcg32 = {0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL};

inline uint32_t pcg32_random_r(pcg32_random_t* rng) {
  uint64_t oldstate = rng->state;
  // Advance internal state
  rng->state = oldstate * 6364136223846793005ULL + (rng->inc | 1);
  // Calculate output function (XSH RR), uses old state for max ILP
  uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
  uint32_t rot = oldstate >> 59u;
  return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

inline f32 rand01() {
  return ldexp(pcg32_random_r(&pcg32), -32);
}
#endif

//
// Numerical
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

inline f32 radians(f32 degrees) {
  return degrees * M_PI / 180.0f;
}

inline vec3 randomPointInUnitSphere() {
  vec3 result;

  do {
    result = 2.0f * vec3(rand01(), rand01(), rand01()) - vec3(1, 1, 1);
  } while (length2(result) >= 1.0f);

  return result;
}

inline vec3 randomPointInUnitDisk() {
  vec3 result;

  do {
    result = 2.0f * vec3(rand01(), rand01(), 0) - vec3(1, 1, 0);
  } while (length2(result) >= 1.0f);

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

  AABB() : minPoint(0, 0, 0), maxPoint(0, 0, 0) {}

  AABB(math::vec3 minPoint, math::vec3 maxPoint)
      : minPoint(minPoint), maxPoint(maxPoint) {}

  bool hit(const math::Ray& ray, f32 tMin, f32 tMax) const;
};

bool AABB::hit(const math::Ray& ray, f32 tMin, f32 tMax) const {
#if SSE
  __m128 one4 = _mm_set1_ps(1.0f);
  __m128 invD4 = _mm_div_ps(one4, ray.direction.v);
  __m128 tMin4 = _mm_set1_ps(tMin);
  __m128 tMax4 = _mm_set1_ps(tMax);

  __m128 t0 =
      _mm_min_ps(_mm_mul_ps(_mm_sub_ps(minPoint.v, ray.origin.v), invD4),
                 _mm_mul_ps(_mm_sub_ps(maxPoint.v, ray.origin.v), invD4));

  __m128 t1 =
      _mm_max_ps(_mm_mul_ps(_mm_sub_ps(minPoint.v, ray.origin.v), invD4),
                 _mm_mul_ps(_mm_sub_ps(maxPoint.v, ray.origin.v), invD4));

  tMin4 = _mm_max_ps(t0, tMin4);
  tMax4 = _mm_min_ps(t1, tMax4);

  /*
    NOTE(johan): Trying to find the component-wise max of tMin4, ignoring the
    4th component which is unused in vec3. There might be a faster way.

    Where digits are components of the __m128, and digits next to each other
    indicate the components were max'd together:
     original = 0 1 2 3
     max1     = 2 1 0 0
     max2     = 02, 11, 20, 30
     max3     = 11, 02, 02, 02
     max4     = 0211, 1102, 2002, 3002
                ^^^^
                This is the one we want, it has 0, 1, 2 in it and will come
                out with mm_cvtss_f32.
  */
  __m128 max1 = _mm_shuffle_ps(tMin4, tMin4, _MM_SHUFFLE(0, 0, 1, 2));
  __m128 max2 = _mm_max_ps(tMin4, max1);
  __m128 max3 = _mm_shuffle_ps(max2, max2, _MM_SHUFFLE(0, 0, 0, 1));
  __m128 max4 = _mm_max_ps(max2, max3);
  tMin = _mm_cvtss_f32(max4);

  // NOTE(johan): Same thing as above, but the min of the components in tMax4
  __m128 min1 = _mm_shuffle_ps(tMax4, tMax4, _MM_SHUFFLE(0, 0, 1, 2));
  __m128 min2 = _mm_min_ps(tMax4, min1);
  __m128 min3 = _mm_shuffle_ps(min2, min2, _MM_SHUFFLE(0, 0, 0, 1));
  __m128 min4 = _mm_min_ps(min2, min3);
  tMax = _mm_cvtss_f32(min4);

  if (tMax <= tMin)
    return false;

  return true;

#else
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
#endif
}

AABB surround(const AABB& box0, const AABB& box1) {
  math::vec3 minPoint(math::min(box0.minPoint.x, box1.minPoint.x),
                      math::min(box0.minPoint.y, box1.minPoint.y),
                      math::min(box0.minPoint.z, box1.minPoint.z));
  math::vec3 maxPoint(math::max(box0.maxPoint.x, box1.maxPoint.x),
                      math::max(box0.maxPoint.y, box1.maxPoint.y),
                      math::max(box0.maxPoint.z, box1.maxPoint.z));
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
