#pragma once
#include "Matrix4x4.h"
#include <Vector3.h>

// ベクトルと行列の乗算
Vector3 TransferNormal(const Vector3& v, const Matrix4x4& m);

// ベクトルの正規化
Vector3 Normalize(const Vector3& vector);

// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

/// <summary>
/// ベクトル同士の加算の代入演算子オーバーロード
/// </summary>
Vector3 operator+=(Vector3 v1, Vector3 v2);

/// <summary>
/// ベクトル同士の加算の二項演算子オーバーロード
/// </summary>
Vector3 operator+(const Vector3& v1, const Vector3& v2);

/// <summary>
/// ベクトル同士の減算の代入演算子オーバーロード
/// </summary>
Vector3 operator-=(const Vector3& v1, const Vector3& v2);

/// <summary>
/// ベクトル同士の減算の二項演算子オーバーロード
/// </summary>
Vector3 operator-(const Vector3& v1, const Vector3& v2);

/// <summary>
/// ベクトル同士の乗算の代入演算子オーバーロード
/// </summary>
Vector3 operator*=(Vector3 v1, Vector3 v2);

/// <summary>
/// ベクトル同士の乗算の二項演算子オーバーロード
/// </summary>
Vector3 operator*(const Vector3& v1, const Vector3& v2);

/// <summary>
/// ベクトル同士の除算の代入演算子オーバーロード
/// </summary>
Vector3 operator/=(Vector3 v1, Vector3 v2);

/// <summary>
/// ベクトル同士の除算の二項演算子オーバーロード
/// </summary>
Vector3 operator/(const Vector3& v1, const Vector3& v2);

/// <summary>
/// ベクトルとfloat型変数の加算の代入演算子オーバーロード
/// <summary>
Vector3 operator+=(Vector3 v, float s);

/// <summary>
/// ベクトルとfloat型変数の加算の二項演算子オーバーロード
/// <summary>
Vector3 operator+(Vector3 v, float s);

/// <summary>
/// ベクトルとfloat型変数の減算の代入演算子オーバーロード
/// <summary>
Vector3 operator-=(Vector3 v, float s);

/// <summary>
/// ベクトルとfloat型変数の減算の二項演算子オーバーロード
/// <summary>
Vector3 operator-(Vector3 v, float s);

/// <summary>
/// ベクトルとfloat型変数の乗算の代入演算子オーバーロード
/// <summary>
Vector3 operator*=(Vector3 v, float s);

/// <summary>
/// ベクトルとfloat型変数の乗算の二項演算子オーバーロード
/// <summary>
Vector3 operator*(Vector3 v, float s);

/// <summary>
/// ベクトルとfloat型変数の除算の代入演算子オーバーロード
/// <summary>
Vector3 operator/=(Vector3 v, float s);

/// <summary>
/// ベクトルとfloat型変数の除算の二項演算子オーバーロード
/// <summary>
Vector3 operator/(Vector3 v, float s);

// 内積
float Dot(const Vector3& v1, const Vector3& v2);

// ベクトルの長さ
float Length(const Vector3& v);

/// <summary>
/// 線形補完
/// </summary>
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

// ベクトル間の距離
Vector3 Distance(const Vector3& v1, const Vector3& v2);