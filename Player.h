#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include <ViewProjection.h>
#include "Input.h"
#include "PlayerBullet.h"
#include <list>
#include "MathUtility.h"
#include "Sprite.h"
#include "Collider.h"
#include "CollisionConfig.h"

//// ゲームシーンの前方宣言
//class GameScene;

/// <summary>
/// 自キャラ
/// </summary>
class Player : public Collider
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void Initialize(Model* model, uint32_t textureHandle, const Vector3& position);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const ViewProjection viewProjection);

	/// <summary>
	/// 旋回
	/// </summary>
	void Rotate();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション（参照渡し）</param>
	void Draw(const ViewProjection viewProjection);

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	/// <summary>
	/// 自キャラのワールド座標を取得
	/// </summary>
	Vector3 GetWorldPosition() override{
		return {
		    worldTransform_.matWorld_.m[3][0], 
			worldTransform_.matWorld_.m[3][1],
		    worldTransform_.matWorld_.m[3][2]
		};
	}

	/// <summary>
	/// 衝突を感知したら呼び出されるコールバック関数 
	/// </summary>
	void OnCollision() override;

	// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() { return bullets_; }

	/// <summary>
	/// 半径を取得
	/// </summary>
	const float GetRadius() { return radius_; }
	const float radius_ = 1.0f;

	/// <summary>
	/// 親となるワールドトランスフォームをセット
	/// </summary>
	/// <param name="parent>親となるワールドトランスフォーム</param>
	void SetParent(const WorldTransform* parent);

	/// <summary>
	/// 3Dレティクルのワールド座標取得
	/// </summary>
	Vector3 Get3DReticleWorldPosition();

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI();

private:
	// 自機のワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// 弾
	//PlayerBullet* bullets_ = nullptr;
	std::list<PlayerBullet*> bullets_;

	// 2Dレティクル用スプライト
	Sprite* sprite2DReticle_ = nullptr;

	// 3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;

	// ビュープロジェクション
	//ViewProjection* viewProjection_;

	// キーボード入力
	Input* input_ = nullptr;

	// ゲームシーン
	//GameScene* gameScene_ = nullptr;
};