#pragma once
#include <Vector3.h>
#include "Matrix4x4.h"

// ベクトルと行列の乗算
Vector3 TransferNormal(const Vector3& v, const Matrix4x4& m);

// ベクトルの正規化
Vector3 Normalize(const Vector3& vector);

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
Vector3 operator-=(Vector3 v1, Vector3 v2);

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
