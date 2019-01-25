#pragma once

typedef float f32;
typedef double f64;
typedef uint8_t u8;
typedef uint32_t u32;
typedef int32_t s32;

#define INTERFACE(c)                \
 public:                            \
  virtual ~c(){};                   \
                                    \
 private:                           \
  c(const c&) = delete;             \
  c(c&&) = delete;                  \
  c& operator=(const c&) = delete;  \
  c& operator=(const c&&) = delete; \
                                    \
 protected:                         \
  c(){};                            \
                                    \
 public:
