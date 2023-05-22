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
	delete debugCamera_;
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

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_);

	// 敵の生成
	enemy_ = new Enemy();
	// 敵の速度
	const float kEnemySpeed = -1.0f;
	Vector3 velocity(0, 0, kEnemySpeed);
	// 敵の初期化
	enemy_->Initialize(model_, { 10.0f, 2.0f, 50.0f }, velocity);

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

#ifdef _DEBUG
	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());
#endif
}

void GameScene::Update() {

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

	
	if (isActiveDebugCamera_) {
	
		debugCamera_->Update();
		//viewProjection_.matView = ;
		//viewProjection_.matProjection = ;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateProjectionMatrix();
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
	player_->Draw(debugCamera_->GetViewProjection());

	// 敵の描画(Enemyがnullでないときだけ)
	if (enemy_) {
		enemy_->Draw(viewProjection_);
	}
	
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
