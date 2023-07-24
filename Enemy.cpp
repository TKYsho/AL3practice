#include "Enemy.h"
#include <cassert>
#include <cmath>
#include <GameScene.h>
#include "Player.h"

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

	// 衝突属性を設定
	SetCollisionAttribute(0x00000010);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(0x11111101);
}

Enemy::~Enemy() {
	//for (EnemyBullet* bullet : bullets_) {
	//	delete bullet;
	//}
	for (TimedCall* timedCall : timedCalls_) {
		delete timedCall;
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

	//// デスフラグの立った弾を削除 (ラムダ式)
	//bullets_.remove_if([](EnemyBullet* bullet) {
	//	if (bullet->IsDead()) {
	//		delete bullet;
	//		return true;
	//	}
	//	return false;
	//});

	// メンバ関数テーブル内の関数を呼び出す
	if (static_cast<size_t>(phase_) == 0) {
		(this->*spPhase[0])();
	} else if(static_cast<size_t>(phase_) == 1) {
		(this->*spPhase[1])();
	}

	//// 弾更新
	//for (EnemyBullet* bullet : bullets_) {
	//	bullet->Update();
	//}

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();
}

void Enemy::Draw(const ViewProjection& viewProjection) {

	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	//// 弾の描画
	//for (EnemyBullet* bullet : bullets_) {
	//	bullet->Draw(viewProjection);
	//}
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

	// 終了した弾発射タイマーを削除
	timedCalls_.remove_if([](TimedCall* timedCall) {
		if (timedCall->IsFinished()) {
			delete timedCall;
			return true;
		}
		return false;
	});

	// 範囲forでリストの全要素について回す(弾発射)
	for (TimedCall* timedCall : timedCalls_) {
		timedCall->Update();
	}

	// 既定の位置に到達したら離脱フェーズへ
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}
}

void Enemy::LeaveUpdate() {

	// 時限発動リストをクリアする
	timedCalls_.clear();

	// 死亡処理
	if (deathTimer_-- == 0) {
		isDead_ = true;
	}

	// 座標を移動させる
	worldTransform_.translation_.x += leaveVelocity_.x;
	worldTransform_.translation_.y += leaveVelocity_.y;
	worldTransform_.translation_.z += leaveVelocity_.z;
}

void Enemy::Fire() {

	// 弾の速度
	const float kBulletSpeed = -0.04f;
	//Vector3 velocity(0, 0, kBulletSpeed);

	// 自キャラのワールド座標を取得する
	Vector3 playerPos = player_->GetWorldPosition();
	// 距離を求める
	Vector3 distance{
		worldTransform_.translation_.x - playerPos.x,
		worldTransform_.translation_.y - playerPos.y,
		worldTransform_.translation_.z - playerPos.z
	};
	// distanceを正規化する
	Normalize(distance);
	// ベクトルの長さを速さに合わせる
	Vector3 velocity{
	    distance.x * kBulletSpeed, 
		distance.y * kBulletSpeed, 
		distance.z * kBulletSpeed
	};

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	//// 弾を登録する
	//bullets_.push_back(newBullet);
	gameScene_->AddEnemyBullet(newBullet);
}

Vector3 Enemy::Normalize(const Vector3& vector) { 
	float length = sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	assert(length);
	Vector3 dir = { vector.x / length, vector.y / length, vector.z / length };
	return dir;
}

void Enemy::ApproachInitialize() {
	// 発射タイマーを初期化
	fireInterval = kFireInterval;

	//ShotReset();
}

void Enemy::OnCollision() {
	// 何もしない
}

void Enemy::ShotReset() {
	// 弾を発射する
	Fire();

	// 発射タイマーをリセットする
	timedCalls_.push_back(new TimedCall(std::bind(&Enemy::ShotReset, this), fireInterval));
}
