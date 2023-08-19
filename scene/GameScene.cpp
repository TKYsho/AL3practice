#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "ImguiManager.h"
#include "AxisIndicator.h"
#include "EnemyBullet.h"
#include <fstream>

GameScene::GameScene() {}

GameScene::~GameScene() {

	delete model_;
	delete player_;
	delete modelPlayer_;
	//delete enemy_;
	for (Enemy* enemy : enemys_) {
		delete enemy;
	}
	delete skydome_;
	delete modelSkydome_;
	delete debugCamera_;
	delete railCamera_;
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
	delete collisionManager_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("car/car.png");

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

	// レティクルのテクスチャ
	TextureManager::Load("target.png");

	// 自キャラの生成
	player_ = new Player();
	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());
	// 自キャラの位置をレールカメラから離す
	Vector3 playerPosition{0.0f, -4.5f, 25.0f};
	// 自キャラ3Dモデルの生成
	modelPlayer_ = Model::CreateFromOBJ("car", true);
	// 自キャラの初期化
	player_->Initialize(modelPlayer_, textureHandle_, playerPosition);

	// 敵の生成
	LoadEnemyPopData();

	//enemy_ = new Enemy();
	//// 敵の速度
	//const float kEnemySpeed = -1.0f;
	//Vector3 velocity(0, 0, kEnemySpeed);
	//// 敵の初期化
	//enemy_->Initialize(model_, { 10.0f, 2.0f, 50.0f }, velocity);
	//// 敵を登録する
	//enemys_.push_back(enemy_);
	//// 敵キャラにゲームシーンを渡す
	//enemy_->SetGameScene(this);
	//// 敵に自キャラのアドレスを渡す
	//enemy_->SetPlayer(player_);
	
	//AddEnemy({10.0f, 2.0f, 50.0f});

	// 天球3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, {0, 0, 0});

	// 衝突マネージャの生成
	collisionManager_ = new CollisionManager();
	
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

	// 衝突マネージャのリストをクリア
	collisionManager_->ClearCollider();

	// 各当たり判定をチェック
	CheckAllCollisions();

	// 自キャラの更新
	player_->Update(viewProjection_);

	// 敵発生コマンド(csv)の更新
	UpdateEnemypopCommands();

	// デスフラグの立った敵を削除（ラムダ式）
	enemys_.remove_if([](Enemy* enemy) {
		if (enemy->GetIsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

	// 敵の更新(Enemyがnullでないときだけ)
	//if (enemy_){
	//	enemy_->Update();
	//}
	for (Enemy* enemy : enemys_) {
		enemy->Update();
	}

	// デスフラグの立った敵弾を削除 (ラムダ式)
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->GetIsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// 敵弾更新
	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
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
	//if (enemy_) {
	//	enemy_->Draw(viewProjection_);
	//}
	for (Enemy* enemy : enemys_) {
		enemy->Draw(viewProjection_);
	}
	
	// 敵弾の描画
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(viewProjection_);
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

	player_->DrawUI();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::AddEnemy(Vector3 pos) {
	// 敵の生成
	Enemy* newEnemy = new Enemy();
	// 敵の速度
	const float kEnemySpeed = -1.0f;
	Vector3 velocity(0, 0, kEnemySpeed);
	// 敵の初期化
	newEnemy->Initialize(model_, pos, velocity);
	
	// 敵キャラにゲームシーンを渡す
	newEnemy->SetGameScene(this);
	// 敵に自キャラのアドレスを渡す
	newEnemy->SetPlayer(player_);

	// 敵を登録する
	enemys_.push_back(newEnemy);
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに敵弾を登録する
	bullets_.push_back(enemyBullet);
}

void GameScene::CheckAllCollisions() {
	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	// 敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = bullets_;
	// 敵リストの取得
	const std::list<Enemy*>& enemys = enemys_;

	// コライダーをリストに追加
	collisionManager_->GetCollider(player_);
	// 敵すべて
	for (Enemy* enemy : enemys) {
		collisionManager_->GetCollider(enemy);
	}
	// 自弾すべて
	for (PlayerBullet* playerBullet : playerBullets) {
		collisionManager_->GetCollider(playerBullet);
	}
	// 敵弾すべて
	for (EnemyBullet* enemyBullet : enemyBullets) {
		collisionManager_->GetCollider(enemyBullet);
	}

	// 当たり判定処理
	collisionManager_->CheckAllCollisions();
}

//float GameScene::GetDistance(const Vector3& posA, const Vector3& posB) { 
//	float distance = 
//		(posB.x - posA.x) * (posB.x - posA.x) + 
//		(posB.y - posA.y) * (posB.y - posA.y) +
//		(posB.z - posA.z) * (posB.z - posA.z);
//	return distance;
//}

void GameScene::LoadEnemyPopData() {
	// ファイルを開く
	std::ifstream file;
	file.open("Resources/enemyPop.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemypopCommands() {
	// 待機処理
	if (isWait_) {
		waitTimer_--;
		if (waitTimer_ <= 0) {
			// 待機完了
			isWait_ = false;
		}
		return;
	}

	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyPopCommands, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		// ,区切りで行の先端文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 敵を発生させる
			AddEnemy(Vector3(x, y, z));
		}

		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待機開始
			isWait_ = true;
			waitTimer_ = waitTime;

			// コマンドループを抜ける
			break;	// 重要
		}
	}
}

void GameScene::GetViewProjection(ViewProjection viewProjection) { 
	viewProjection = viewProjection_;
}

//void GameScene::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
//	// 衝突フィルタリング
//	if (colliderA->GetCollisionAttribute() != colliderB->GetCollisionMask() || colliderB->GetCollisionAttribute() != colliderA->GetCollisionMask()) {
//		return;
//	}
//	
//	float distance = GetDistance(colliderA->GetWorldPosition(), colliderB->GetWorldPosition());
//	// 球と球の交差判定
//	if (distance <= (colliderA->GetRadius() + colliderB->GetRadius()) *
//	                    (colliderA->GetRadius() + colliderB->GetRadius())) {
//		// コライダーAの衝突時コールバックを呼び出す
//		colliderA->OnCollision();
//		// コライダーBの衝突時コールバックを呼び出す
//		colliderB->OnCollision();
//	}
//}
