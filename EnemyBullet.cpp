#include "EnemyBullet.h"
#include <cassert>
#include "MathUtility.h"

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	
	// NULLチェック
	assert(model);
	// 引数で受け取ったモデルをメンバ変数に代入
	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("cube.jpg");
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
	// 弾の形を設定（z方向に伸びた形状）
	worldTransform_.scale_.x = 0.5f;
	worldTransform_.scale_.y = 0.5f;
	worldTransform_.scale_.z = 3.0f;
	// v引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;

	// 衝突属性を設定
	SetCollisionAttribute(0x00000010);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(0x11111101);
}

void EnemyBullet::Update() {

	// Y軸周り角度（θy）
	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
	// X軸周り角度（θx）
	Vector3 velocityXZ{velocity_.x, 0.0f, velocity_.z};
	worldTransform_.rotation_.x = std::atan2(-velocity_.y, Length(velocityXZ));

	// 座標を移動させる（1フレーム分の移動量を足し込む）(必ず行列更新の上に記載)
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void EnemyBullet::Draw(const ViewProjection& viewProjection) {

	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

void EnemyBullet::OnCollision() {
	// 死亡フラグを立てる
	isDead_ = true;
}
