#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugCamera.h"
#include "Player.h"
#include "Enemy.h"
#include "Skydome.h"
#include "RailCamera.h"
#include <sstream>
#include "Collider.h"
#include "CollisionManager.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 敵を追加
	/// </summary>
	/// <param name="pos">座標</param>
	void AddEnemy(Vector3 pos);

	/// <summary>
	/// 敵弾を追加する
	/// </summary>
	/// <param name="enemyBullet">敵弾</param>
	void AddEnemyBullet(EnemyBullet* enemyBullet);

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();

	/// <summary>
	/// AとBの距離を取得
	/// </summary>
	/// float GetDistance(const Vector3& posA, const Vector3& posB);

	/// <summary>
	/// 敵発生データの読み込み(csvから)
	/// </summary>
	void LoadEnemyPopData();

	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemypopCommands();

	/// <summary>
	/// ビュープロジェクションの取得
	/// </summary>
	void GetViewProjection(ViewProjection viewProjection);

	/// <summary>
	/// ビュープロジェクションの取得
	/// </summary>
	ViewProjection* GetViewProjection() { return &viewProjection_; };

private: // メンバ関数
	/// <summary>
	/// コライダー2つの衝突判定と応答
	/// </summary>
	/// <param name="colliderA">コライダーA</param>
	/// <param name="colliderB">コライダーB</param>
	/// void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 3Dモデル
	Model* model_ = nullptr;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 自キャラ
	Player* player_ = nullptr;
	// 自キャラモデル
	Model* modelPlayer_ = nullptr;

	// 敵キャラ
	//Enemy* enemy_ = nullptr;
	std::list<Enemy*> enemys_;

	// 敵弾のリスト
	std::list<EnemyBullet*> bullets_;

	// 天球
	Skydome* skydome_ = nullptr;
	// 天球3Dモデル
	Model* modelSkydome_ = nullptr;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	// デバッグカメラ有効フラグ
	bool isActiveDebugCamera_ = false;

	// レールカメラ
	RailCamera* railCamera_ = nullptr;

	// 敵発生コマンド(csvから)
	std::stringstream enemyPopCommands;

	// 敵待機フラグ
	bool isWait_;

	// 敵待機タイマー
	int waitTimer_;

	// 衝突マネージャ
	CollisionManager* collisionManager_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
