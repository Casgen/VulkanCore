#pragma once

#include <cstdio>
#include <emmintrin.h>
#include <immintrin.h>

struct Vec3i
{
    union {
        struct
        {
            float x, y, z;
        };

        __m128i simd;
    };

    Vec3i(const int x, const int y, const int z) : x(x), y(y), z(z)
    {
        simd = _mm_set_epi32(0.f, z, y, x);
    }

    Vec3i(const int x) : x(x), y(x), z(x)
    {
        simd = _mm_set_epi32(0.f, x, x, x);
    }

    Vec3i() : x(0), y(0), z(0)
    {
        simd = _mm_setzero_si128();
    }

    Vec3i operator*(Vec3i const& other) const
    {
        return Vec3i(_mm_mul_epi32(this->simd, other.simd));
    }

    Vec3i operator+(Vec3i const& other) const
    {
        return Vec3i(_mm_add_epi32(this->simd, other.simd));
    }

    Vec3i operator-(Vec3i const& other) const
    {
        return Vec3i(_mm_sub_epi32(this->simd, other.simd));
    }

    Vec3i& operator*=(Vec3i const& other)
    {
        return *this = Vec3i(_mm_mul_epi32(this->simd, other.simd));
    }

    Vec3i& operator+=(Vec3i const& other)
    {
        return *this = Vec3i(_mm_add_epi32(this->simd, other.simd));
    }

    Vec3i& operator-=(Vec3i const& other)
    {
        return *this = Vec3i(_mm_sub_epi32(this->simd, other.simd));
    }


    float Dot(const Vec3i& other)
    {
        __m128 mul = _mm_mul_epi32(simd, other.simd);

        __m128 firstAdd = _mm_hadd_epi32(mul, mul);
        __m128 secondAdd = _mm_hadd_epi32(firstAdd, firstAdd);

        return secondAdd[3];
    }

	static Vec3i Max(const Vec3i& lhs, const Vec3i rhs) {
		return Vec3i(_mm_max_epi32(lhs.simd, rhs.simd));
	}

	static Vec3i Min(const Vec3i& lhs, const Vec3i rhs) {
		return Vec3i(_mm_min_epi32(lhs.simd, rhs.simd));
	}

    Vec3i Cross(const Vec3i& other)
    {
        __m256 firstMul = _mm256_set_epi32(0.f, 0.f, y, x, x, z, z, y);
        __m256 secondMul = _mm256_set_epi32(0.f, 0.f, other.x, other.y, other.z, other.x, other.y, other.z);

        __m256 mul = _mm256_mul_epi32(firstMul, secondMul);

        __m128 firstSub = _mm_set_epi32(0.f, mul[4], mul[2], mul[0]);
        __m128 secondSub = _mm_set_epi32(0.f, mul[5], mul[3], mul[1]);

        __m128 sub = _mm_sub_epi32(firstSub, secondSub);

        return Vec3i(sub);
    }

    void Print()
    {
        std::printf("{%.4f, %.4f, %.4f}", x, y, z);
    }

  private:
    Vec3i(const __m128i& simd) : x(simd[0]), y(simd[1]), z(simd[2])
    {
        this->simd = simd;
    }
};
