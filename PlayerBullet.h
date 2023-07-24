#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include <ViewProjection.h>
#include "Collider.h"

/// <summary>
/// 自キャラの弾
/// </summary>
class PlayerBullet : public Collider {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="position">初期座標</param>
	/// <param name="velocity">速度</param>
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="vuewProjection">ビュープロジェクション</param>
	void Draw(const ViewProjection& viewProjection);

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	Vector3 GetWorldPosition() override {
		return {
		    worldTransform_.matWorld_.m[3][0], 
			worldTransform_.matWorld_.m[3][1],
		    worldTransform_.matWorld_.m[3][2]
		};
	}

	/// <summary>
	/// デスフラグのgetter 
	/// </summary>
	bool IsDead() const { return isDead_; }	// constを書くことでメンバ変数の書き換えを防ぐ

	/// <summary>
	/// 衝突を感知したら呼び出されるコールバック関数
	/// </summary>
	void OnCollision() override;

	/// <summary>
	/// 半径を取得
	/// </summary>
	const float GetRadius() { return radius_; }
	const float radius_ = 1.0f;

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// 速度
	Vector3 velocity_;
	
	// 寿命<frm>
	static const int32_t kLifeTime = 60 * 5;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;
};
