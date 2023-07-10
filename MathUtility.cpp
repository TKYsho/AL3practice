#include "MathUtility.h"
#include <cassert>

// ベクトルと行列の乗算
Vector3 TransferNormal(const Vector3& v, const Matrix4x4& m) { 
	
	Vector3 result{
	    v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
	    v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]
	};

	return result; 
}

// ベクトルの正規化
Vector3 Normalize(const Vector3& vector) {

	float length = sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	assert(length);
	Vector3 dir = {vector.x / length, vector.y / length, vector.z / length};
	return dir;
}

// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) { 
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

// ベクトル同士の加算の代入演算子オーバーロード
Vector3 operator+=(Vector3 v1, Vector3 v2) {
	Vector3 result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	v1 = result;
	return v1;
}

// ベクトル同士の加算の二項演算子オーバーロード
Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	Vector3 result = v1;
	return result += v2;
}

// ベクトル同士の減算の代入演算子オーバーロード
Vector3 operator-=(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}

// ベクトル同士の減算の二項演算子オーバーロード
Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	Vector3 result = v1;
	result -= v2;
	return result;
}

// ベクトル同士の乗算の代入演算子オーバーロード
Vector3 operator*=(Vector3 v1, Vector3 v2) {
	Vector3 result;
	result.x = v1.x * v2.x;
	result.y = v1.y * v2.y;
	result.z = v1.z * v2.z;
	v1 = result;
	return v1;
}

// ベクトル同士の乗算の二項演算子オーバーロード
Vector3 operator*(const Vector3& v1, const Vector3& v2) {
	Vector3 result = v1;
	return result *= v2;
}

// ベクトル同士の除算の代入演算子オーバーロード
Vector3 operator/=(Vector3 v1, Vector3 v2) {
	Vector3 result;
	result.x = v1.x / v2.x;
	result.y = v1.y / v2.y;
	result.z = v1.z / v2.z;
	v1 = result;
	return v1;
}

// ベクトル同士の除算の二項演算子オーバーロード
Vector3 operator/(const Vector3& v1, const Vector3& v2) {
	Vector3 result = v1;
	return result /= v2;
}

Vector3 operator+=(Vector3 v, float s) { 
	Vector3 result;
	result.x = v.x + s;
	result.y = v.y + s;
	result.z = v.z + s;
	return result;
}

Vector3 operator+(Vector3 v, float s) { 
	Vector3 result = v;
	return result += s;
}

Vector3 operator-=(Vector3 v, float s) { 
	Vector3 result;
	result.x = v.x - s;
	result.y = v.y - s;
	result.z = v.z - s;
	return result;
}

Vector3 operator-(Vector3 v, float s) { 
	Vector3 result = v;
	return result -= s;
}

Vector3 operator*=(Vector3 v, float s) { 
	Vector3 result;
	result.x = v.x * s;
	result.y = v.y * s;
	result.z = v.z * s;
	return result;
}

Vector3 operator*(Vector3 v, float s) { 
	Vector3 result = v;
	return result *= s;
}

Vector3 operator/=(Vector3 v, float s) { 
	Vector3 result;
	result.x = v.x / s;
	result.y = v.y / s;
	result.z = v.z / s;
	return result;
}

Vector3 operator/(Vector3 v, float s) { 
	Vector3 result = v;
	return result /= s;
}

// 内積
float Dot(const Vector3& v1, const Vector3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

// ベクトルの長さ
float Length(const Vector3& v) { return std::sqrt(Dot(v, v)); }

// 線形補完
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {
	Vector3 temp;
	temp = (v1 * (1.0f - t)) + (v2 * t);
	return temp;
}

Vector3 Distance(const Vector3& v1, const Vector3& v2) { 
	return Vector3{v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; 
}
