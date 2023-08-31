#include "RailCamera.h"
#include "ImGuiManager.h"

void RailCamera::Initialize(const Vector3& position, const Vector3& radian) {

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// 引数で受け取った初期座標をメンバ変数に代入
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = radian;

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void RailCamera::Update() {

	// 移動距離の定義
	Vector3 move = {0.0f, 0.0f, 0.0f};
	// 回転角の定義
	Vector3 rot = {0.0f, 0.0f, 0.0f};

	// 移動距離の加算
	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;
	worldTransform_.translation_.z += move.z;

	// 回転角の加算
	worldTransform_.rotation_.x += rot.x;
	worldTransform_.rotation_.y += rot.y;
	worldTransform_.rotation_.z += rot.z;

	// ワールド行列の更新
	worldTransform_.UpdateMatrix();

	// カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = Inverse(worldTransform_.matWorld_);

	// ビュー行列の転送
	viewProjection_.TransferMatrix();

	#ifdef _DEBUG
	// カメラの座標を画面表示する処理
	ImGui::Begin("Camera");
	ImGui::SliderFloat("transform.x", &worldTransform_.translation_.x, -100.0f, 100.0f);
	ImGui::SliderFloat("transform.y", &worldTransform_.translation_.y, -100.0f, 100.0f);
	ImGui::SliderFloat("transform.z", &worldTransform_.translation_.z, -100.0f, 100.0f);
	ImGui::SliderFloat("rotation.x", &worldTransform_.rotation_.x, -10.0f, 10.0f);
	ImGui::SliderFloat("rotation.y", &worldTransform_.rotation_.y, -10.0f, 10.0f);
	ImGui::SliderFloat("rotation.z", &worldTransform_.rotation_.z, -10.0f, 10.0f);
	ImGui::End();
	#endif
}
