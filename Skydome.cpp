#include "Skydome.h"

void Skydome::Initialize(Model* model, const Vector3& position) {
	// NULLポインタチェック
	assert(model);
	model_ = model;

	// ワールドトランスフォームの更新
	worldTransform_.Initialize();

	// 引数で受け取った初期座標をメンバ変数に代入
	worldTransform_.translation_ = position;
}

void Skydome::Update() {
	// ワールドトランスフォーム更新
	worldTransform_.UpdateMatrix();
}

void Skydome::Draw(ViewProjection& viewProjection) {
	// 3Dモデル描画
	model_->Draw(worldTransform_, viewProjection);
}
