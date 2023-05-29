#include "Player.h"
#include <cassert>
#include "ImGuiManager.h"

void Player::Initialize(Model* model, uint32_t textureHandle){

	// NULLポインタチェック
	assert(model);

	// 受け渡し
	model_ = model;
	textureHandle_ = textureHandle;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
}

Player::~Player() {

	// bullet_の開放
	//delete bullets_;
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Player::Update() {

	// デスフラグの立った弾を削除 (ラムダ式)
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクター移動速度
	const float kCharacterSpeed = 0.2f;

	// 押した方向で移動ベクトルを変更（左右）
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}
	if (input_->PushKey(DIK_LEFT) && input_->PushKey(DIK_RIGHT)) {
		move.x = 0;
	}

	// 押した方向で移動ベクトルを変更（上下）
	if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	}
	if (input_->PushKey(DIK_UP) && input_->PushKey(DIK_DOWN)) {
		move.y = 0;
	}

	// キーボード入力による移動制限
	// 移動限界座標
	const float kMoveLimitX = 34.0f;
	const float kMoveLimitY = 18.0f;

	// 範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	// 旋回処理
	Rotate();

	// 座標移動（ベクトルの加算）
	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;
	worldTransform_.translation_.z += move.z;

	// キャラクター攻撃処理
	Attack();

	// 弾更新
	//if (bullets_) {
	//	bullets_->Update();
	//}
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	// ワールドトランスフォームの更新(下二つの処理の代わりになる)
	worldTransform_.UpdateMatrix();

	//// ワールド行列にアフィン変換行列を代入
	//worldTransform_.matWorld_ = MakeAffineMatrix(
	//    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	//// 行列を定数バッファに転送
	//worldTransform_.TransferMatrix();

	
#ifdef _DEBUG // デバッグ時のみデバッグウィンドウを表示（Update関数の中に実装する）

	// キャラクターの座標を画面表示する処理
	// ウィンドウのサイズ、座標の指定
	ImGui::SetNextWindowPos({0, 0});
	ImGui::SetNextWindowSize({300, 100});

	// 座標の設定
	ImGui::Begin("Player");
	float sliderValue[3] = {
	    worldTransform_.translation_.x, worldTransform_.translation_.y,
	    worldTransform_.translation_.z};
	ImGui::SliderFloat3("position", sliderValue, -20.0f, 20.0f);
	worldTransform_.translation_ = {sliderValue[0], sliderValue[1], sliderValue[2]};
	ImGui::End();

#endif // _DEBUG
}

void Player::Rotate() {

	// 回転速さ[ラジアン/frame]
	//const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= 0.02f;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += 0.02f;
	}
}

void Player::Attack() {

	// spaceキーで弾を発射
	if (input_->TriggerKey(DIK_SPACE)) {

		//// 既に弾があればメモリを開放する
		// if (bullets_) {
		//	delete bullets_;
		//	bullets_ = nullptr;
		// }

		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		// 速度ベクトルを自機の向きに合わせて回転させる
		velocity = TransformNormal(velocity, worldTransform_.matWorld_);

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, worldTransform_.translation_, velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);
	}
}

Vector3 Player::GetWorldPosition() { 
	//// ワールド座標を入れる変数
	//Vector3 worldPos;
	//// ワールド行列の平行移動行列を取得（ワールド座標）
	//worldPos.x = worldTransform_.translation_.x;
	//worldPos.y = worldTransform_.translation_.y;
	//worldPos.z = worldTransform_.translation_.z;
	
	return worldTransform_.translation_;
}

void Player::OnCollision() {
	// 何もしない
}

void Player::Draw(const ViewProjection viewProjection) {

	// 3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	// 弾描画
	//if (bullets_) {
	//	bullets_->Draw(viewProjection);
	//}
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}
