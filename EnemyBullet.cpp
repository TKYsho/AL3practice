#include "EnemyBullet.h"
#include <cassert>
#include "Player.h"

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	
	// NULLチェック
	assert(model);
	// 引数で受け取ったモデルをメンバ変数に代入
	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("cube.jpg");
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// 引数で受け取った座標をワールドトランスフォームに代入
	worldTransform_.translation_ = position;
	// 弾の形を設定（z方向に伸びた形状）
	worldTransform_.scale_.x = 0.5f;
	worldTransform_.scale_.y = 0.5f;
	worldTransform_.scale_.z = 3.0f;
	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;
}

void EnemyBullet::Update() {

	//// 敵弾から自キャラへのベクトルを計算
	//Vector3 toPlayer = worldTransform_.translation_ - player_->GetWorldPosition();
	//// ベクトルを正規化する
	//Normalize(toPlayer);
	//Normalize(velocity_);
	//// 球面線形補完により、今の速度と自キャラへのベクトルを内挿し、新たな速度とする
	//velocity_ = Lerp(velocity_, toPlayer, 0.5f) * -0.02f;
	
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
