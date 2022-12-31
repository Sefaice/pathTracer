#pragma once

// #include <math.h>
#include <algorithm>
#include <iostream>
#include <iomanip>

// #define M_PI 3.14159265358979323846f
#define INV_PI 1 / M_PI

#define INF std::numeric_limits<float>::infinity()

// basic structs

struct vec2 {
	float x, y;

	vec2() : x(0), y(0) {}
	vec2(float _x, float _y) : x(_x), y(_y) {}
	void print() {
		std::cout << std::fixed << std::setprecision(3);
		std::cout << "vec2(" << x << ", " << y << ")" << std::endl;
		std::cout << std::setprecision(0);
	}
};

struct vec3 {
	float x, y, z;

	vec3() : x(0), y(0), z(0) {}
	vec3(float _a) : x(_a), y(_a), z(_a) {}
	vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	void print() {
		std::cout << std::fixed << std::setprecision(3);
		std::cout << "vec3(" << x << ", " << y << ", " << z << ")" << std::endl;
		std::cout << std::setprecision(0);
	}
	float length() {
		return sqrt(x * x + y * y + z * z);
	}
	float lengthSquared() {
		return x * x + y * y + z * z;
	}
};

struct vec4 {
	float x, y, z, w;

	vec4() : x(0), y(0), z(0), w(0) {}
	vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	vec4(vec3 _v, float _w) : x(_v.x), y(_v.y), z(_v.z), w(_w) {}
	void print() {
		std::cout << std::fixed << std::setprecision(3);
		std::cout << "vec4(" << x << ", " << y << ", " << z << ", " << w << ")" << std::endl;
		std::cout << std::setprecision(0);
	}
};

struct mat3 {
	float m[3][3];

	mat3() {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				m[i][j] = 0;
	}
	mat3(float _x) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				m[i][j] = 0;
			}
			m[i][i] = _x;
		}
	}
	mat3(float _x1, float _x2, float _x3, 
		float _x4, float _x5, float _x6, 
		float _x7, float _x8, float _x9) {
		m[0][0] = _x1; m[0][1] = _x2; m[0][2] = _x3;
		m[1][0] = _x4; m[1][1] = _x5; m[1][2] = _x6;
		m[2][0] = _x7; m[2][1] = _x8; m[2][2] = _x9;
	}
	mat3(vec3 _c1, vec3 _c2, vec3 _c3) {
		m[0][0] = _c1.x; m[0][1] = _c1.y; m[0][2] = _c1.z;
		m[1][0] = _c2.x; m[1][1] = _c2.y; m[1][2] = _c2.z;
		m[2][0] = _c3.x; m[2][1] = _c3.y; m[2][2] = _c3.z;
	}
	void print() {
		std::cout << std::fixed << std::setprecision(3);
		std::cout << "mat3x3(";
		for (int i = 0; i < 3; i++) {
			std::cout << "(" << m[i][0] << ", " << m[i][1] << ", " << m[i][2] << ")";
			if (i != 2) std::cout << ", ";
		}
		std::cout << ")" << std::endl;
		std::cout << std::setprecision(0);
	}
};

struct mat4 {  // COLUMN-MAJOR. m[0] is actually the first column of matrix 
	float m[4][4];

	mat4() {
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				m[i][j] = 0;
	}
	mat4(float _x) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				m[i][j] = 0;
			}
			m[i][i] = _x;
		}
	}
	mat4(float _x1, float _x2, float _x3, float _x4,
		float _x5, float _x6, float _x7, float _x8,
		float _x9, float _x10, float _x11, float _x12,
		float _x13, float _x14, float _x15, float _x16) {
		m[0][0] = _x1; m[0][1] = _x2; m[0][2] = _x3; m[0][3] = _x4;
		m[1][0] = _x5; m[1][1] = _x6; m[1][2] = _x7; m[1][3] = _x8;
		m[2][0] = _x9; m[2][1] = _x10; m[2][2] = _x11; m[2][3] = _x12;
		m[3][0] = _x13; m[3][1] = _x14; m[3][2] = _x15; m[3][3] = _x16;
	}
	void print() {
		std::cout << std::fixed << std::setprecision(3);
		std::cout << "mat4x4(";
		for (int i = 0; i < 4; i++) {
			std::cout << "(" << m[i][0] << ", " << m[i][1] << ", " << m[i][2] << ", " << m[i][3] << ")";
			if (i != 3) std::cout << ", ";
		}
		std::cout << ")" << std::endl;
		std::cout << std::setprecision(0);
	}
};

struct Ray {
    vec3 pos;
    float min_t;
    vec3 dir;
    float time;         // time of this ray for motion blur
    float max_t;

    unsigned int mask;  // ray mask
    unsigned int id;    // ray ID
    unsigned int flags; // ray flags

    Ray(vec3 _pos, vec3 _dir, float _min_t, float _max_t): pos(_pos), dir(_dir), min_t(_min_t), max_t(_max_t) {}
};

// operators

inline vec2 operator*(const vec2& v, float a) {
	return vec2(v.x * a, v.y * a);
}


inline vec2 operator+(const vec2& v1, float a) {
	return vec2(v1.x + a, v1.y + a);
}

inline vec2 operator+(const vec2& v1, const vec2& v2) {
	return vec2(v1.x + v2.x, v1.y + v2.y);
}

inline vec2 operator*(const vec2& v1, const vec2& v2) {
	return vec2(v1.x * v2.x, v1.y * v2.y);
}

inline vec3 operator-(const vec3& v) {
	return vec3(-v.x, -v.y, -v.z);
}

inline vec3 operator+(const vec3& v1, const vec3& v2) {
	return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline vec3 operator-(const vec3& v1, const vec3& v2) {
	return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline vec3 operator*(const vec3& v, float a) {
	return vec3(v.x * a, v.y * a, v.z * a);
}

inline vec3 operator*(const vec3& v1, const vec3& v2) {
	return vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

inline vec3 operator/(const vec3& v, float a) {
	return vec3(v.x / a, v.y / a, v.z / a);
}

inline vec3 operator/(const vec3& v1, const vec3 &v2) {
	return vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

inline vec3 operator*(const mat3& m, const vec3& v) {
	return vec3(m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z,
		m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z,
		m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z);
}

inline vec4 operator*(const mat4& m, const vec4& v) {
	return vec4(m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0] * v.w,
		m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1] * v.w,
		m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2] * v.w,
		m.m[0][3] * v.x + m.m[1][3] * v.y + m.m[2][3] * v.z + m.m[3][3] * v.w);
}

inline mat3 operator/(const mat3& m, float a) {
	mat3 r;
	if (a == 0) {
		printf("Divide error!\n");
		return r;
	}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			r.m[i][j] = m.m[i][j] / a;
		}
	}

	return r;
}

inline mat4 operator*(const mat4& m, float a) {
	mat4 r;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			r.m[i][j] = m.m[i][j] * a;
		}
	}

	return r;
}

inline mat4 operator*(const mat4& m1, const mat4& m2) {
	mat4 r;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			r.m[j][i] = m1.m[0][i] * m2.m[j][0] + m1.m[1][i] * m2.m[j][1] + m1.m[2][i] * m2.m[j][2] + m1.m[3][i] * m2.m[j][3];
		}
	}

	return r;
}

inline mat4 operator/(const mat4& m, float a) {
	mat4 r;
	if (a == 0) {
		printf("Divide error!\n");
		return r;
	}
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			r.m[i][j] = m.m[i][j] / a;
		}
	}

	return r;
}

// functions

inline float maxInTwo(float a, float b) {
	return a > b ? a : b;
}

//inline float maxInThree(float a, float b, float c) {
//	return a > b ? std::max(a, c) : std::max(b, c);
//}
//
//inline float minInThree(float a, float b, float c) {
//	return a < b ? std::min(a, c) : std::min(b, c);
//}
inline float maxInThree(float a, float b, float c) {
	return a > b ? (a > c ? a : c) : (b > c ? b : c);
}

inline float minInThree(float a, float b, float c) {
	return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

inline vec3 maxInTwo(const vec3& v1, const vec3& v2) {
	return vec3(maxInTwo(v1.x, v2.x), maxInTwo(v1.y, v2.y), maxInTwo(v1.z, v2.z));
}

inline vec3 normalize(vec3& v) {
	float length = v.length();
	if (length == 0)
		return vec3(0);
	return vec3(v.x / length, v.y / length, v.z / length);
}

inline float dot(const vec3& v1, const vec3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline vec3 cross(const vec3& v1, const vec3& v2) {
	return vec3(v1.y * v2.z - v2.y * v1.z, v2.x * v1.z - v1.x * v2.z, v1.x * v2.y - v2.x * v1.y);
}

inline vec3 pow(const vec3& v1, const vec3& e) {
	return vec3(pow(v1.x, e.x), pow(v1.y, e.y), pow(v1.z, e.z));
}

// // 求反射向量的方向，v方向是从光源指向碰撞点，n必须为单位向量（v不用）
// inline vec3 reflect(const vec3& v, const vec3& n) {
// 	return vec3(v - n * dot(v, n) * 2);
// }

// // 折射。入射向量v，法向量n都要是单位向量，出射向量也是单位向量，nint为折射率（入射材质折射率/折射材质折射率）
// inline vec3 refract(const vec3& v, const vec3& n, float nint) {
// 	// 入射角
// 	float dt = dot(v, n);
// 	float discr = 1.0f - nint * nint * (1 - dt * dt);
// 	// glm的实现也有这个判断，可能是为了排除折射率小于1的情况
// 	if (discr > 0)
// 	{
// 		vec3 outRefracted =  (v - n * dt) * nint - n * sqrtf(discr);
// 		return outRefracted;
// 	}

// 	return vec3(0);
// }

inline vec2 lerp(vec2 v1, vec2 v2, float t) {
	return vec2(v1.x * (1 - t) + v2.x * t, v1.y * (1 - t) + v2.y * t);
}

inline vec3 lerp(vec3 v1, vec3 v2, float t) { // v3 = v1 * (1 - t) + v2 * t
	return vec3(v1.x * (1 - t) + v2.x * t, v1.y * (1 - t) + v2.y * t,
		v1.z * (1 - t) + v2.z * t);
}

inline vec4 lerp(vec4 v1, vec4 v2, float t) {
	return vec4(v1.x * (1 - t) + v2.x * t, v1.y * (1 - t) + v2.y * t,
		v1.z * (1 - t) + v2.z * t, v1.w * (1 - t) + v2.w * t);
}

inline mat3 lerp(mat3 m1, mat3 m2, float t) {
	mat3 m;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			m.m[i][j] = m1.m[i][j] * (1 - t) + m2.m[i][j] * t;
	return m;
}

inline float radians(float degree) {
	return degree * M_PI / 180.0f;
}

inline vec3 vector3(const vec4 v) {
	return vec3(v.x, v.y, v.z);
}

inline vec4 vector4(const vec3 v) {
	return vec4(v.x, v.y, v.z, 0);
}

inline mat3 matrix3(const mat4 m) {
	mat3 r;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			r.m[i][j] = m.m[i][j];

	return r;
}

inline mat4 matrix4(const mat3 m) {
	mat4 r(1.0f); // position 4,4 should be 1 !
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			r.m[i][j] = m.m[i][j];

	return r;
}

inline mat4 transpose(const mat4& m) {
	mat4 r;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			r.m[i][j] = m.m[j][i];
		}
	}

	return r;
}

inline mat3 inverse(const mat3& m) {
	mat3 adjoint;
	adjoint.m[0][0] = +(m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1]);
	adjoint.m[0][1] = -(m.m[0][1] * m.m[2][2] - m.m[0][2] * m.m[2][1]);
	adjoint.m[0][2] = +(m.m[0][1] * m.m[1][2] - m.m[0][2] * m.m[1][1]);
	adjoint.m[1][0] = -(m.m[1][0] * m.m[2][2] - m.m[1][2] * m.m[2][0]);
	adjoint.m[1][1] = +(m.m[0][0] * m.m[2][2] - m.m[0][2] * m.m[2][0]);
	adjoint.m[1][2] = -(m.m[0][0] * m.m[1][2] - m.m[0][2] * m.m[1][0]);
	adjoint.m[2][0] = +(m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0]);
	adjoint.m[2][1] = -(m.m[0][0] * m.m[2][1] - m.m[0][1] * m.m[2][0]);
	adjoint.m[2][2] = +(m.m[0][0] * m.m[1][1] - m.m[0][1] * m.m[1][0]);

	float determinant = m.m[0][0] * adjoint.m[0][0] + m.m[0][1] * adjoint.m[1][0] + m.m[0][2] * adjoint.m[2][0];

	if (determinant == 0.0f) {
		printf("Inverse does not exist!\n");
		return mat3();
	}

	return adjoint / determinant;
}

inline mat4 inverse(const mat4& m) {
	float subfactor00 = m.m[2][2] * m.m[3][3] - m.m[3][2] * m.m[2][3]; // 00/10's minor
	float subfactor01 = m.m[2][1] * m.m[3][3] - m.m[3][1] * m.m[2][3];
	float subfactor02 = m.m[2][1] * m.m[3][2] - m.m[3][1] * m.m[2][2];
	float subfactor03 = m.m[1][2] * m.m[3][3] - m.m[3][2] * m.m[1][3]; // 20
	float subfactor04 = m.m[1][1] * m.m[3][3] - m.m[3][1] * m.m[1][3];
	float subfactor05 = m.m[1][1] * m.m[3][2] - m.m[3][1] * m.m[1][2];
	float subfactor06 = m.m[1][2] * m.m[2][3] - m.m[2][2] * m.m[1][3]; // 30
	float subfactor07 = m.m[1][1] * m.m[2][3] - m.m[2][1] * m.m[1][3];
	float subfactor08 = m.m[1][1] * m.m[2][2] - m.m[2][1] * m.m[1][2];
	float subfactor09 = m.m[2][0] * m.m[3][3] - m.m[3][0] * m.m[2][3]; // 01/11
	float subfactor10 = m.m[2][0] * m.m[3][2] - m.m[3][0] * m.m[2][2];
	float subfactor11 = m.m[1][2] * m.m[3][3] - m.m[3][2] * m.m[1][3]; // 21
	float subfactor12 = m.m[1][0] * m.m[3][3] - m.m[3][0] * m.m[1][3];
	float subfactor13 = m.m[1][0] * m.m[3][2] - m.m[3][0] * m.m[1][2];
	float subfactor14 = m.m[1][0] * m.m[2][3] - m.m[2][0] * m.m[1][3]; // 31
	float subfactor15 = m.m[1][0] * m.m[2][2] - m.m[2][0] * m.m[1][2];
	float subfactor16 = m.m[2][0] * m.m[3][1] - m.m[3][0] * m.m[2][1]; // 02/12
	float subfactor17 = m.m[1][0] * m.m[3][1] - m.m[3][0] * m.m[1][1]; // 22
	float subfactor18 = m.m[1][0] * m.m[2][1] - m.m[2][0] * m.m[1][1]; // 32

	mat4 adjoint;
	adjoint.m[0][0] = +(m.m[1][1] * subfactor00 - m.m[1][2] * subfactor01 + m.m[1][3] * subfactor02);
	adjoint.m[0][1] = -(m.m[0][1] * subfactor00 - m.m[0][2] * subfactor01 + m.m[0][3] * subfactor02);
	adjoint.m[0][2] = +(m.m[0][1] * subfactor03 - m.m[0][2] * subfactor04 + m.m[0][3] * subfactor05);
	adjoint.m[0][3] = -(m.m[0][1] * subfactor06 - m.m[0][2] * subfactor07 + m.m[0][3] * subfactor08);
	adjoint.m[1][0] = -(m.m[1][0] * subfactor00 - m.m[1][2] * subfactor09 + m.m[1][3] * subfactor10);
	adjoint.m[1][1] = +(m.m[0][0] * subfactor00 - m.m[0][2] * subfactor09 + m.m[0][3] * subfactor10);
	adjoint.m[1][2] = -(m.m[0][0] * subfactor11 - m.m[0][2] * subfactor12 + m.m[0][3] * subfactor13);
	adjoint.m[1][3] = +(m.m[0][0] * subfactor06 - m.m[0][2] * subfactor14 + m.m[0][3] * subfactor15);
	adjoint.m[2][0] = +(m.m[1][0] * subfactor01 - m.m[1][1] * subfactor09 + m.m[1][3] * subfactor16);
	adjoint.m[2][1] = -(m.m[0][0] * subfactor01 - m.m[0][1] * subfactor09 + m.m[0][3] * subfactor16);
	adjoint.m[2][2] = +(m.m[0][0] * subfactor04 - m.m[0][1] * subfactor12 + m.m[0][3] * subfactor17);
	adjoint.m[2][3] = -(m.m[0][0] * subfactor07 - m.m[0][1] * subfactor14 + m.m[0][3] * subfactor18);
	adjoint.m[3][0] = -(m.m[1][0] * subfactor02 - m.m[1][1] * subfactor10 + m.m[1][2] * subfactor16);
	adjoint.m[3][1] = +(m.m[0][0] * subfactor02 - m.m[0][1] * subfactor10 + m.m[0][2] * subfactor16);
	adjoint.m[3][2] = -(m.m[0][0] * subfactor05 - m.m[0][1] * subfactor13 + m.m[0][2] * subfactor17);
	adjoint.m[3][3] = +(m.m[0][0] * subfactor08 - m.m[0][1] * subfactor15 + m.m[0][2] * subfactor18);

	float determinant = m.m[0][0] * adjoint.m[0][0] + m.m[0][1] * adjoint.m[1][0]
		+ m.m[0][2] * adjoint.m[2][0] + m.m[0][3] * adjoint.m[3][0];

	if (determinant == 0.0f) {
		printf("Inverse does not exist!\n");
		return mat4();
	}

	return adjoint / determinant;
}

inline mat4 translate(const mat4& m, const vec3& v) {
	mat4 t = mat4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		v.x, v.y, v.z, 1.0);
	
	return t * m;
}

inline mat4 rotate(const mat4& m, float angle, vec3 axis) { // angle is in radians
	axis = normalize(axis);
	float a = cos(angle / 2);
	float b = sin(angle / 2) * axis.x;
	float c = sin(angle / 2) * axis.y;
	float d = sin(angle / 2) * axis.z;
	mat4 r = mat4(1 - 2 * c * c - 2 * d * d, 2 * b * c + 2 * a * d,     2 * b * d - 2 * a * c,     0,
				  2 * b * c - 2 * a * d,     1 - 2 * b * b - 2 * d * d, 2 * a * b + 2 * c * d,     0,
				  2 * a * c + 2 * b * d,     2 * c * d - 2 * a * b,     1 - 2 * b * b - 2 * c * c, 0,
				  0,                         0,                         0,                         1);

	return r * m;
}

// angle in radians
inline mat4 getRotate(float angle, vec3 axis) {
	axis = normalize(axis);
	float a = cos(angle / 2);
	float b = sin(angle / 2) * axis.x;
	float c = sin(angle / 2) * axis.y;
	float d = sin(angle / 2) * axis.z;
	mat4 r = mat4(1 - 2 * c * c - 2 * d * d, 2 * b * c + 2 * a * d, 2 * b * d - 2 * a * c, 0,
		2 * b * c - 2 * a * d, 1 - 2 * b * b - 2 * d * d, 2 * a * b + 2 * c * d, 0,
		2 * a * c + 2 * b * d, 2 * c * d - 2 * a * b, 1 - 2 * b * b - 2 * c * c, 0,
		0, 0, 0, 1);

	return r;
}

inline mat4 scale(const mat4& m, float scaleAmount) {
	mat4 s = mat4(scaleAmount, 0, 0, 0,
		0, scaleAmount, 0, 0,
		0, 0, scaleAmount, 0,
		0, 0, 0, 1);

	return s * m;
}

inline mat4 scale(const mat4& m, float fx, float fy, float fz) {
	mat4 s = mat4(fx, 0, 0, 0,
		0, fy, 0, 0,
		0, 0, fz, 0,
		0, 0, 0, 1);

	return s * m;
}

inline mat4 lookAt(vec3 eye, vec3 center, vec3 worldUp) {
    vec3 front = center - eye;
	front = normalize(front);
	vec3 right = cross(front, worldUp);
	right = normalize(right);
	vec3 up = cross(right, front);
	up = normalize(up);

	mat4 view1 = mat4(right.x, up.x, -front.x, 0,
		right.y, up.y, -front.y, 0,
		right.z, up.z, -front.z, 0,
		0, 0, 0, 1);
	mat4 view2 = mat4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-eye.x, -eye.y, -eye.z, 1);

	return view1 * view2;
}

// fov in degrees, ar is aspect ratio, near, far
inline mat4 perspective(float fov, float ar, float n, float f) {
	float tanHalfHFOV = tanf(radians(fov / 2.0f)) * ar;
	float tanHalfVFOV = tanf(radians(fov / 2.0f));
	float r = n * tanHalfHFOV, l = -n * tanHalfHFOV;
	float t = n * tanHalfVFOV, b = -n * tanHalfVFOV;

	return mat4(2 * n / (r - l),   0,                 0,                    0,
		        0,                 2 * n / (t - b),   0,                    0,
		        (r + l) / (r - l), (t + b) / (t - b), -(f + n) / (f - n),   -1,
		        0,                 0,                 -2 * f * n / (f - n), 0);
}

// left, right, bottom, top, near, far
inline mat4 ortho(float l, float r, float b, float t, float n, float f) {

	return mat4(2 / (r - l),        0,                  0,                  0,
		        0,                  2 / (t - b),        0,                  0,
		        0,                  0,                  -2 / (f - n),       0,
		        -(r + l) / (r - l), -(t + b) / (t - b), -(f + n) / (f - n), 1);
}