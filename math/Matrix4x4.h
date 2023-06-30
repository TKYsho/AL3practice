#pragma once
#include "Vector3.h"
#include <cmath>

/// <summary>
/// 4x4行列
/// </summary>
struct Matrix4x4 final {
	float m[4][4];
};

// スケーリング行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

// X軸の回転行列
Matrix4x4 MakeRotationXMatrix(float theta);

// Y軸の回転行列
Matrix4x4 MakeRotationYMatrix(float theta);

// Z軸の回転行列
Matrix4x4 MakeRotationZMatrix(float theta);

// 平行移動行列
Matrix4x4 MaakeTranslateMatrix(Vector3 trans);

// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m);

// アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate);

// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

// 代入演算子オーバーロード
Matrix4x4& operator*=(Matrix4x4& m1, const Matrix4x4& m2);

// 二項演算子オーバーロード
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);
