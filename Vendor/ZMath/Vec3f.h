#include <cstdio>
#include <immintrin.h>
#include <ostream>
#include <xmmintrin.h>

struct Vec3f {

	union {
		struct {
			float x, y, z;
		};

		__m128 simd;
	};

	Vec3f(const float x, const float y, const float z) : x(x), y(y), z(z) {
		simd = _mm_set_ps(0.f, z, y ,x);
	}

	Vec3f(const float x) : x(x), y(x), z(x) {
		simd = _mm_set_ps(0.f, x, x ,x);
	}

	Vec3f() : x(0.f), y(0.f), z(0.f) {
		simd = _mm_setzero_ps();
	}


	Vec3f operator*(const Vec3f& other) {
		return Vec3f(_mm_mul_ps(this->simd, other.simd));
	}

	Vec3f operator+(const Vec3f& other) {
		return Vec3f(_mm_add_ps(this->simd, other.simd));
	}
	
	Vec3f operator-(const Vec3f& other) {
		return Vec3f(_mm_sub_ps(this->simd, other.simd));
	}

	Vec3f operator/(const Vec3f& other) {
		return Vec3f(_mm_div_ps(this->simd, other.simd));
	}

	float Dot(const Vec3f& other) {
		__m128 mul = _mm_mul_ps(simd, other.simd);

		__m128 firstAdd = _mm_hadd_ps(mul, mul);
		__m128 secondAdd = _mm_hadd_ps(firstAdd, firstAdd);
	

		return secondAdd[3];
	}

	Vec3f Cross(const Vec3f& other) {
		__m256 firstMul = _mm256_set_ps(0.f, 0.f, y, x, x, z, z, y);
		__m256 secondMul = _mm256_set_ps(0.f,0.f, other.x, other.y, other.z, other.x, other.y, other.z);

		__m256 mul = _mm256_mul_ps(firstMul, secondMul);
		
		__m128 firstSub = _mm_set_ps(0.f, mul[4], mul[2], mul[0]);
		__m128 secondSub = _mm_set_ps(0.f, mul[5], mul[3], mul[1]);

		__m128 sub = _mm_sub_ps(firstSub, secondSub);

		return Vec3f(sub);
	}

	void Print() {
		std::printf("{%.4f, %.4f, %.4f}", x, y, z);
	}
	

private:
	Vec3f(const __m128& simd) : x(simd[0]), y(simd[1]), z(simd[2]) {
		this->simd = simd;
	}

};
