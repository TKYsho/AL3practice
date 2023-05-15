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
}

void Enemy::Update() {

	// 座標を移動させる
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();
}

void Enemy::Draw(const ViewProjection& viewProjection) {

	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}
