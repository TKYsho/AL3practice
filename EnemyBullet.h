#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include <ViewProjection.h>
#include "MathUtility.h"

// Playerクラス前方宣言
class Player;

/// <summary>
/// 敵の弾
/// </summary>
class EnemyBullet {
public:

	/// <summary>
	///	初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summmary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection);

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	Vector3 GetWorldPosition() { return worldTransform_.translation_; }

	/// <summary>
	/// デスフラグのゲッター
	/// </summary>
	bool GetIsDead() const { return isDead_; }

	/// <summary>
	/// 衝突を感知したら呼び出されるコールバック関数
	/// </summary>
	void OnCollision();

	/// <summary>
	/// 半径を取得
	/// </summary>
	const float GetRadius() { return radius_; }
	const float radius_ = 1.0f;

	// player取得関数
	void SetPlayer(Player* player) { player_ = player; }

private:
	// ワールドトランスフォーム
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

	// player
	Player* player_ = nullptr;
};