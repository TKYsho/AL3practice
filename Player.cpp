#include "Player.h"
#include <cassert>
#include "ImGuiManager.h"
#include "MathUtility.h"
//#include <GameScene.h>

void Player::Initialize(Model* model, uint32_t textureHandle, const Vector3& position){

	// NULLポインタチェック
	assert(model);

	// 受け渡し
	model_ = model;
	textureHandle_ = textureHandle;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("target.png");

	// スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, {0, 0}, {1, 1, 1,1}, {0.5, 0.5});

	// 引数で受け取った値をセット
	worldTransform_.translation_ = position;

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	// 3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();

	// ビュープロジェクションをゲームシーンから取得
	//viewProjection_ = gameScene_->GetViewProjection();

	// 衝突属性を設定
	SetCollisionAttribute(0x00000001);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(0x11111110);
}

Player::~Player() {

	// bullet_の開放
	//delete bullets_;
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
	// Sprite::Createは内部でnew処理を行っているためdeleteが必要
	delete sprite2DReticle_;
}

void Player::Update(const ViewProjection viewProjection) {

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

	
	// 3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	{
		Vector3 positionReticle = Get3DReticleWorldPosition();

		// ビューポート行列
		Matrix4x4 matViewport =
		    MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

		// ビュー行列とプロジェクション行列、ビューポート行列を合成する
		Matrix4x4 matViewProjectionViewport =
		    viewProjection.matView * viewProjection.matProjection * matViewport;

		// ワールド→スクリーン座標変換（ここで3Dから2Dになる）
		positionReticle = Transform(positionReticle, matViewProjectionViewport);

		// スプライトのレティクルに座標設定
		sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));
	}

	// 自機のワールド座標から3Dレティクルのワールド座標を計算
	{
		// 自機から3Dレティクルへの距離
		const float kDistancePlayerTo3DReticle = 50.0f;
		// 自機から3Dレティクルへのオフセット(Z+向き)
		Vector3 offset = { 0, 0, 1.0f };
		// 自機のワールド行列の回転を反映
		offset = TransferNormal(offset, worldTransform_.matWorld_);
		// ベクトルの長さを整える
		offset = Normalize(offset) * kDistancePlayerTo3DReticle;
		// 3Dレティクルの座標を設定
		worldTransform3DReticle_.translation_ = GetWorldPosition() + offset;
		worldTransform3DReticle_.UpdateMatrix();
	}

	// マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
	{
		POINT mousePosition;
		// マウス座標（スクリーン座標）を取得する
		GetCursorPos(&mousePosition);

		// クライアントエリア座標に変換する(クライアント座標とはスクリーン座標のこと)
		HWND hwnd = WinApp::GetInstance()->GetHwnd();
		ScreenToClient(hwnd, &mousePosition);

		// マウス座標を2Dレティクルのスプライトに代入
		sprite2DReticle_->SetPosition( {(float)mousePosition.x, (float)mousePosition.y} );

		// ビューポート行列
		Matrix4x4 matViewport =
		    MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

		// ビュープロジェクションビューポート合成行列
		Matrix4x4 matVPV = viewProjection.matView * viewProjection.matProjection * matViewport;
		// 合成行列の逆行列を計算する
		Matrix4x4 matInverseVPV = Inverse(matVPV);

		// スクリーン座標
		Vector3 posNear = Vector3((float)mousePosition.x, (float)mousePosition.y, 0);	// 二アークリップ
		Vector3 posFar = Vector3((float)mousePosition.x, (float)mousePosition.y, 1);	// ファークリップ
	
		// スクリーン座標からワールド座標系へ
		posNear = Transform(posNear, matInverseVPV);
		posFar = Transform(posFar, matInverseVPV);
	
		// マウスレイの方向
		Vector3 mouseDirection;
		mouseDirection.x = posNear.x - posFar.x;
		mouseDirection.y = posNear.y - posFar.y;
		mouseDirection.z = posNear.z - posFar.z;

		mouseDirection = Normalize(mouseDirection);
		// カメラから照準オブジェクトへの距離
		const float kDistanceTestObject = -80.0f;
		// posNearを基準に3Dレティクルの座標を求める
		worldTransform3DReticle_.translation_ = posNear + (mouseDirection * kDistanceTestObject);
		// ワールド行列の更新
		worldTransform3DReticle_.UpdateMatrix();

		// デバッグ文字
		ImGui::Begin("Player");
		ImGui::Text("2DReticle:(%f,%f)", sprite2DReticle_->GetPosition().x, sprite2DReticle_->GetPosition().y);
		ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", posNear.x, posNear.y, posNear.z);
		ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", posFar.x, posFar.y, posFar.z);
		ImGui::Text("3DReticle:(%+.2f,%+.2f,%+.2f)", worldTransform3DReticle_.translation_.x,
		    worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);
		ImGui::End();
	}

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

	// //3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	//{ 
	//	Vector3 positionReticle = worldTransform3DReticle_.translation_;
	//	
	//	// ビューポート行列
	//	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	//
	//	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	//	Matrix4x4 matViewProjectionViewport = viewProjection.matView * viewprojection.matProjection * matViewport;

	//	// ワールド→スクリーン座標変換（ここで3Dから2Dになる）
	//	positionReticle = Transform(positionReticle, matViewProjectionViewport);

	//	// スプライトのレティクルに座標設定
	//	sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));
	//}

	
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

		// 自機から照準オブジェクトへのベクトル
		velocity = Get3DReticleWorldPosition() - GetWorldPosition();
		velocity = Normalize(velocity) * kBulletSpeed;

		//// 速度ベクトルを自機の向きに合わせて回転させる
		//velocity = TransferNormal(velocity, worldTransform_.matWorld_);

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, GetWorldPosition(), velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);
	}
}

void Player::OnCollision() {
	// 何もしない
}

void Player::SetParent(const WorldTransform* parent) { 
	// 親となるワールドトランスフォームをセット
	worldTransform_.parent_ = parent; 
}

Vector3 Player::Get3DReticleWorldPosition() { 
	Vector3 worldPos;
	worldPos.x = worldTransform3DReticle_.matWorld_.m[3][0];
	worldPos.y = worldTransform3DReticle_.matWorld_.m[3][1];
	worldPos.z = worldTransform3DReticle_.matWorld_.m[3][2];
	return worldPos; 
}

void Player::DrawUI() {
	sprite2DReticle_->Draw(); 
}

void Player::Draw(const ViewProjection viewProjection) {

	// 3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	//// 3Dレティクルを仮で描画
	//model_->Draw(worldTransform3DReticle_, viewProjection);

	// 弾描画
	//if (bullets_) {
	//	bullets_->Draw(viewProjection);
	//}
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}
