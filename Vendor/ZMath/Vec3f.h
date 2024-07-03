#pragma once

#include <cmath>
#include <cstdio>
#include <immintrin.h>
#include <smmintrin.h>
#include <xmmintrin.h>

/**
 * This is a SIMD accelerated 3-component vector.
 * DO NOT USE THIS VECTOR IN VULKAN AS THE SIZE IS NOT 12 BYTES.
 */
struct Vec3f
{
    union {
        struct
        {
            float x, y, z;
        };

        __m128 simd;
    };

    Vec3f(const float x, const float y, const float z) : x(x), y(y), z(z)
    {
        simd = _mm_set_ps(0.f, z, y, x);
    }

    Vec3f(const float x) : x(x), y(x), z(x)
    {
        simd = _mm_set_ps(0.f, x, x, x);
    }

	Vec3f(const Vec3f& other) : x(other.x), y(other.y), z(other.z) {
        simd = _mm_set_ps(0.f, other.z, other.y, other.x);
	}

    Vec3f() : x(0.f), y(0.f), z(0.f)
    {
        simd = _mm_setzero_ps();
    }

    Vec3f operator*(Vec3f const& other) const
    {
        return Vec3f(_mm_mul_ps(this->simd, other.simd));
    }

    Vec3f operator+(Vec3f const& other) const
    {
        return Vec3f(_mm_add_ps(this->simd, other.simd));
    }

    Vec3f operator-(Vec3f const& other) const
    {
        return Vec3f(_mm_sub_ps(this->simd, other.simd));
    }

    Vec3f operator/(Vec3f const& other) const
    {
        return Vec3f(_mm_div_ps(this->simd, other.simd));
    }



    Vec3f operator*(float const& value) const
    {
        return Vec3f(_mm_mul_ps(this->simd, _mm_set_ps(0.f, value, value, value)));
    }

    Vec3f operator+(float const& value) const
    {
        return Vec3f(_mm_add_ps(this->simd, _mm_set_ps(0.f, value, value, value)));
    }

    Vec3f operator-(float const& value) const
    {
        return Vec3f(_mm_sub_ps(this->simd, _mm_set_ps(0.f, value, value, value)));
    }

    Vec3f operator/(float const& value) const
    {
        return Vec3f(_mm_div_ps(this->simd, _mm_set_ps(0.f, value, value, value)));
    }



    Vec3f& operator*=(Vec3f const& other)
    {
        return *this = Vec3f(_mm_mul_ps(this->simd, other.simd));
    }

    Vec3f& operator+=(Vec3f const& other)
    {
        return *this = Vec3f(_mm_add_ps(this->simd, other.simd));
    }

    Vec3f& operator-=(Vec3f const& other)
    {
        return *this = Vec3f(_mm_sub_ps(this->simd, other.simd));
    }

    Vec3f& operator/=(Vec3f const& other)
    {
        return *this = Vec3f(_mm_div_ps(this->simd, other.simd));
    }


    Vec3f operator*=(float const& value)
    {
        return *this = Vec3f(_mm_mul_ps(this->simd, _mm_set_ps(0.f, value, value, value)));
    }

    Vec3f operator+=(float const& value)
    {
        return *this = Vec3f(_mm_add_ps(this->simd, _mm_set_ps(0.f, value, value, value)));
    }

    Vec3f operator-=(float const& value)
    {
        return *this = Vec3f(_mm_sub_ps(this->simd, _mm_set_ps(0.f, value, value, value)));
    }

    Vec3f operator/=(float const& value)
    {
        return *this = Vec3f(_mm_div_ps(this->simd, _mm_set_ps(0.f, value, value, value)));
    }

    float Dot(const Vec3f& other)
    {
        __m128 mul = _mm_mul_ps(simd, other.simd);

        __m128 firstAdd = _mm_hadd_ps(mul, mul);
        __m128 secondAdd = _mm_hadd_ps(firstAdd, firstAdd);

        return secondAdd[3];
    }

	static Vec3f Max(const Vec3f& lhs, const Vec3f rhs) {
		return Vec3f(_mm_max_ps(lhs.simd, rhs.simd));
	}

	static Vec3f Min(const Vec3f& lhs, const Vec3f rhs) {
		return Vec3f(_mm_min_ps(lhs.simd, rhs.simd));
	}

	static Vec3f Min(const Vec3f& vec1, const Vec3f& vec2, const Vec3f& vec3) {
		return Min(Min(vec1, vec2), vec3);
	}

	static Vec3f Max(const Vec3f& vec1, const Vec3f& vec2, const Vec3f& vec3) {
		return Max(Max(vec1, vec2), vec3);
	}

    Vec3f Cross(const Vec3f& other)
    {
        __m256 firstMul = _mm256_set_ps(0.f, 0.f, y, x, x, z, z, y);
        __m256 secondMul = _mm256_set_ps(0.f, 0.f, other.x, other.y, other.z, other.x, other.y, other.z);

        __m256 mul = _mm256_mul_ps(firstMul, secondMul);

        __m128 firstSub = _mm_set_ps(0.f, mul[4], mul[2], mul[0]);
        __m128 secondSub = _mm_set_ps(0.f, mul[5], mul[3], mul[1]);

        __m128 sub = _mm_sub_ps(firstSub, secondSub);

        return Vec3f(sub);
    }

	Vec3f Normalize() const {
		float magnitude = Magnitude();
		__m128 magnitudeSimd = _mm_set_ps(0.0f, magnitude, magnitude, magnitude);

		return Vec3f(_mm_div_ps(simd, magnitudeSimd));
	}

	float Magnitude() const {
		__m128 compSquared = _mm_mul_ps(simd, simd);
		
		float sum = simd[0] + simd[1] + simd[2];
		return sqrtf(sum);
	}

    void Print()
    {
        std::printf("{%.4f, %.4f, %.4f}", x, y, z);
    }

  private:
    Vec3f(const __m128& simd) : x(simd[0]), y(simd[1]), z(simd[2])
    {
        this->simd = simd;
    }
};

