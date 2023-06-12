#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "ImguiManager.h"
#include "AxisIndicator.h"

GameScene::GameScene() {}

GameScene::~GameScene() {

	delete model_;
	delete player_;
	delete enemy_;
	delete skydome_;
	delete modelSkydome_;
	delete debugCamera_;
	delete railCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("player1.png");

	// 3Dモデルの生成
	model_ = Model::Create();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// レールカメラの生成
	railCamera_ = new RailCamera();
	// レールカメラの初期化
	railCamera_->Initialize({0.0f, 0.0f, -50.0f}, {0.0f, 0.0f, 0.0f});

	// 自キャラの生成
	player_ = new Player();
	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());
	// 自キャラの位置をレールカメラから離す
	Vector3 playerPosition{0.0f, 0.0f, 50.0f};
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, playerPosition);

	// 敵の生成
	enemy_ = new Enemy();
	// 敵の速度
	const float kEnemySpeed = -1.0f;
	Vector3 velocity(0, 0, kEnemySpeed);
	// 敵の初期化
	enemy_->Initialize(model_, { 10.0f, 2.0f, 50.0f }, velocity);
	// 敵に自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);

	// 天球3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, {0, 0, 0});
	
#ifdef _DEBUG
	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());
#endif
}

void GameScene::Update() {

	// 天球の更新
	skydome_->Update();

	// レールカメラの更新
	railCamera_->Update();

	// 各当たり判定をチェック
	CheckAllCollisions();

	// 自キャラの更新
	player_->Update();

	// 敵の更新(Enemyがnullでないときだけ)
	if (enemy_){
		enemy_->Update();
	}

	// Debug時のみデバッグカメラ有効フラグを切り替える
#ifdef _DEBUG

	// Tキーでフラグ切り替え
	if (input_->TriggerKey(DIK_T)) {

		if (isActiveDebugCamera_) {
			isActiveDebugCamera_ = false;
		} else {
			isActiveDebugCamera_ = true;
		}
	}

	// デバッグテキストでデバッグカメラのON/OFFの状況を表示
	ImGui::Begin("isActiveDebugCamera");
	ImGui::Text("push T : toggle ON / OFF");
	if (isActiveDebugCamera_) {
		ImGui::Text("debugCamera : ON");
	} else {
		ImGui::Text("debugCamera : OFF");
	}
	ImGui::End();

#endif
	
	// デバッグカメラがONのとき
	if (isActiveDebugCamera_) {
		// デバッグカメラ更新処理
		debugCamera_->Update();
		// オブジェクトの各行列を渡す
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else if (!isActiveDebugCamera_) {
		// レールカメラから各行列を渡す
		viewProjection_.matView = railCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
		// 
		viewProjection_.TransferMatrix();
		// ビュープロジェクション行列の更新と転送
		//viewProjection_.UpdateProjectionMatrix();
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	//skydome_->Draw(viewProjection_);

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	//model_->Draw(worldTransform_, viewProjection_, textureHandle_);

	// 自キャラの描画
	player_->Draw(viewProjection_);

	// 敵の描画(Enemyがnullでないときだけ)
	if (enemy_) {
		enemy_->Draw(viewProjection_);
	}
	
	// 天球の描画
	skydome_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckAllCollisions() {
	// 判定対象AとBの座標
	Vector3 posA, posB;

	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	// 敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

	#pragma region 自キャラと敵弾の当たり判定

	// 自キャラの座標
	posA = player_->GetWorldPosition();

	// 自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets) {
		// 敵弾の座標
		posB = bullet->GetWorldPosition();
		// 座標AとBの距離を求める
		float distance = GetDistance(posA, posB);

		// if(距離 <= (半径A + 半径B)²)
		if (distance <= (player_->GetRadius() + bullet->GetRadius()) * (player_->GetRadius() + bullet->GetRadius())) {
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			// 敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}

	#pragma endregion

	#pragma region 自弾と敵キャラの当たり判定

	// 敵キャラの座標
	posA = enemy_->GetWorldPosition();

	// 敵キャラと自弾全ての当たり判定
	for (PlayerBullet* bullet : playerBullets) {
		// 敵弾の座標
		posB = bullet->GetWorldPosition();
		// 座標AとBの距離を求める
		float distance = GetDistance(posA, posB);

		// if(距離 <= (半径A + 半径B)²)
		if (distance <= (enemy_->GetRadius() + bullet->GetRadius()) * (enemy_->GetRadius() + bullet->GetRadius())) {
			// 自キャラの衝突時コールバックを呼び出す
			enemy_->OnCollision();
			// 敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}

	#pragma endregion

	#pragma region 自キャラと敵キャラの当たり判定

	// 自キャラの座標
	posA = player_->GetWorldPosition();
	// 敵キャラの座標
	posB = enemy_->GetWorldPosition();
	// 座標AとBの距離を求める
	float distance = GetDistance(posA, posB);

	// 衝突判定
	if (distance <= (player_->GetRadius() + enemy_->GetRadius()) * (player_->GetRadius() + enemy_->GetRadius())) {
		// 自キャラの衝突時コールバックを呼び出す
		player_->OnCollision();
		// 敵キャラの衝突時コールバックを呼び出す
		enemy_->OnCollision();
	}

	#pragma endregion
}

float GameScene::GetDistance(const Vector3& posA, const Vector3& posB) { 
	float distance = 
		(posB.x - posA.x) * (posB.x - posA.x) + 
		(posB.y - posA.y) * (posB.y - posA.y) +
		(posB.z - posA.z) * (posB.z - posA.z);
	return distance;
}
