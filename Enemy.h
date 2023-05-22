#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include <ViewProjection.h>

/// <summary>
/// 敵
/// </summary>
class Enemy {

	enum class Phase {
		Approach, // 接近する
		Leave     // 離脱する
	};
	// フェーズ
	Phase phase_ = Phase::Approach;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection);

	/// <summary>
	/// 敵接近処理
	/// </summary>
	void ApproachUpdate();

	/// <summary>
	/// 敵離脱処理
	/// </summary>
	void LeaveUpdate();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// 速度
	Vector3 velocity_;

	// 接近時の速度
	Vector3 approachVelocity_{0.0f, 0.0f, -1.0f};

	// 離脱時の速度
	Vector3 leaveVelocity_{-0.1f, 0.1f, 0.1f};

	//// メンバ関数ポインタ
	//void (Enemy::*pApproachUpdate)();

	// メンバ関数ポインタのテーブル
	static void (Enemy::*spPhase[])();
};

