#include "Enemy.h"
#include <cassert>

void Enemy::Initialize(Model* model, const Vector3& position, const Vector3& velocity) { 
	
	// NULLポインタチェック
	assert(model);
	model_ = model;

	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("enemy1.png");

	// ワールドトランスフォームの更新
	worldTransform_.Initialize();
	
	// 引数で受け取った初期座標をメンバ変数に代入
	worldTransform_.translation_ = position;

	// 引数で受け取った速度をセット
	velocity_ = velocity;

	//// 接近処理メンバ関数に関数のアドレスを代入
	//pApproachUpdate = &Enemy::ApproachUpdate;

	// 発射カウントを初期化
	ApproachInitialize();
}

Enemy::~Enemy() {
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
}

// フェーズの関数テーブル
void (Enemy::*Enemy::spPhase[])() = {
    &Enemy::ApproachUpdate, // 接近 要素番号0
    &Enemy::LeaveUpdate     // 離脱 要素番号1
};

void Enemy::Update() {

	// 敵行動パターン
	//switch (phase_) { 
	//case Phase::Approach:	// 接近
	//	//// 接近処理
	//	//ApproachUpdate();
	//	
	//	//// 接近処理メンバ関数ポインタ内の関数を呼び出す
	//	//(this->*pApproachUpdate)();

	//	// メンバ関数テーブル内の関数を呼び出す
	//	(this->*spPhase[0])();
	//	break;

	//case Phase::Leave:		// 離脱
	//	// 離脱処理
	//	LeaveUpdate();
	//	break;
	//}

	// デスフラグの立った弾を削除 (ラムダ式)
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// メンバ関数テーブル内の関数を呼び出す
	if (static_cast<size_t>(phase_) == 0) {
		// メンバ関数テーブル内の関数を呼び出す
		(this->*spPhase[0])();
	} else if(static_cast<size_t>(phase_) == 1) {
		(this->*spPhase[1])();
	}

	// 弾更新
	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();
}

void Enemy::Draw(const ViewProjection& viewProjection) {

	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	// 弾の描画
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

void Enemy::ApproachUpdate() {
	// 座標を移動させる
	worldTransform_.translation_.x += approachVelocity_.x;
	worldTransform_.translation_.y += approachVelocity_.y;
	worldTransform_.translation_.z += approachVelocity_.z;

	// 弾発射
	if (fireInterval-- == 0) {
		Fire();
		fireInterval = kFireInterval;
	}

	// 既定の位置に到達したら離脱フェーズへ
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}
}

void Enemy::LeaveUpdate() {
	// 座標を移動させる
	worldTransform_.translation_.x += leaveVelocity_.x;
	worldTransform_.translation_.y += leaveVelocity_.y;
	worldTransform_.translation_.z += leaveVelocity_.z;
}

void Enemy::Fire() {

	// 弾の速度
	const float kBulletSpeed = -1.0f;
	Vector3 velocity(0, 0, kBulletSpeed);

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	// 弾を登録する
	bullets_.push_back(newBullet);
}

void Enemy::ApproachInitialize() {
	// 発射タイマーを初期化
	fireInterval = kFireInterval;
}
