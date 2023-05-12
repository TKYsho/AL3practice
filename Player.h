#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include <ViewProjection.h>
#include "Input.h"
#include "PlayerBullet.h"
#include <list>

/// <summary>
/// 自キャラ
/// </summary>
class Player
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void Initialize(Model* model, uint32_t textureHandle);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

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

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// 弾
	//PlayerBullet* bullets_ = nullptr;
	std::list<PlayerBullet*> bullets_;

	// キーボード入力
	Input* input_ = nullptr;

};
