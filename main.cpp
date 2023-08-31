#include "Audio.h"
#include "AxisIndicator.h"
#include "DirectXCommon.h"
#include "GameScene.h"
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include "WinApp.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	// 汎用機能
	Input* input = nullptr;
	Audio* audio = nullptr;
	AxisIndicator* axisIndicator = nullptr;
	PrimitiveDrawer* primitiveDrawer = nullptr;
	GameScene* gameScene = nullptr;
	enum scene {
		TITLE,
		TUTORIAL,
		GAME,
		END
	} SCENE = TITLE;
	int timeCount = 0;
	//bool isClear = false;

	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow(L"GC2A_14_タカヤ_ショウ_AL3");

	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

	#pragma region 汎用機能初期化

	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize();

	// オーディオの初期化
	audio = Audio::GetInstance();
	audio->Initialize();

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());
	TextureManager::Load("white1x1.png");

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 3Dモデル静的初期化
	Model::StaticInitialize();

	// 軸方向表示初期化
	axisIndicator = AxisIndicator::GetInstance();
	axisIndicator->Initialize();

	primitiveDrawer = PrimitiveDrawer::GetInstance();
	primitiveDrawer->Initialize();

#pragma endregion

	uint32_t highway_star = 0;
	highway_star = audio->LoadWave("highway-star.wav");
	uint32_t on_the_highway = 0;
	on_the_highway = audio->LoadWave("on-the-highway.wav");
	uint32_t soundHandle = 0;

	gameScene = new GameScene();

	//while (true) {
		switch (SCENE) {
		case TITLE:
		#pragma region タイトル処理

			// ゲームシーンの初期化
			gameScene->Initialize();

		    timeCount = 0;
		    //isClear = false;

			soundHandle = audio->PlayWave(on_the_highway, true);

			while (1) {
				// メッセージ処理
				if (win->ProcessMessage()) {
					break;
				}

				// ImGui受付開始
				imguiManager->Begin();
				// 入力関連の毎フレーム処理
				input->Update();

				//ImGui::Begin("scene");
				//ImGui::Text("Title");
				//ImGui::End();


				// 軸表示の更新
				axisIndicator->Update();
				// ImGui受付終了
				imguiManager->End();

				// 描画開始
				dxCommon->PreDraw();
				// タイトルの描画
				gameScene->DrawTitle();
				// 軸表示の描画
				axisIndicator->Draw();
				// プリミティブ描画のリセット
				primitiveDrawer->Reset();
				// ImGui描画
				imguiManager->Draw();
				// 描画終了
				dxCommon->PostDraw();

				// タイトルの毎フレーム処理
				if (input->TriggerKey(DIK_SPACE)) {
					SCENE = TUTORIAL;
					break;
				}
			}
			#pragma endregion

		case TUTORIAL:
		#pragma region チュートリアル処理
			while (1) {
				// メッセージ処理
				if (win->ProcessMessage()) {
					break;
				}

				// ImGui受付開始
				imguiManager->Begin();
				// 入力関連の毎フレーム処理
				input->Update();

				//ImGui::Begin("scene");
				//ImGui::Text("Tutorial");
				//ImGui::End();

				// 軸表示の更新
				axisIndicator->Update();
				// ImGui受付終了
				imguiManager->End();

				// 描画開始
				dxCommon->PreDraw();
				// チュートリアルの描画
				gameScene->DrawTutorial();
				// 軸表示の描画
				axisIndicator->Draw();
				// プリミティブ描画のリセット
				primitiveDrawer->Reset();
				// ImGui描画
				imguiManager->Draw();
				// 描画終了
				dxCommon->PostDraw();

				// チュートリアルの毎フレーム処理
				if (input->TriggerKey(DIK_SPACE)) {
					// 音声停止
				    audio->StopWave(soundHandle);
					SCENE = GAME;
					break;
				}
			}
		#pragma endregion

		case GAME:
		#pragma region ゲーム処理

			// BGM
		    soundHandle = audio->PlayWave(highway_star, true);

			// メインループ
			while (true) {
				// メッセージ処理
				if (win->ProcessMessage()) {
					break;
				}

				// ImGui受付開始
				imguiManager->Begin();
				// 入力関連の毎フレーム処理
				input->Update();
				// ゲームシーンの毎フレーム処理
				gameScene->Update();
				// 軸表示の更新
				axisIndicator->Update();
				// ImGui受付終了
				imguiManager->End();

				// 描画開始
				dxCommon->PreDraw();
				// ゲームシーンの描画
				gameScene->Draw();
				// 軸表示の描画
				axisIndicator->Draw();
				// プリミティブ描画のリセット
				primitiveDrawer->Reset();
				// ImGui描画
				imguiManager->Draw();
				// 描画終了
				dxCommon->PostDraw();
				// カウント加算
			    timeCount++;
			    if (timeCount >= 2830) {
				    //isClear = true;
					// 音声停止
				    audio->StopWave(soundHandle);
				    SCENE = END;
				    break;
				}
				if (gameScene->IsPlayerAlive() == false) {
				    //isClear = false;
				    //  音声停止
				    audio->StopWave(soundHandle);
					SCENE = END;
					break;
				}
			}
		#pragma endregion

		case END:
		#pragma region 終了処理

			soundHandle = audio->PlayWave(on_the_highway, true);

			while (1) {
				// メッセージ処理
				if (win->ProcessMessage()) {
					break;
				}

				// ImGui受付開始
				imguiManager->Begin();
				// 入力関連の毎フレーム処理
				input->Update();

				//ImGui::Begin("scene");
				//ImGui::Text("End");
				//ImGui::End();

				// 軸表示の更新
				axisIndicator->Update();
				// ImGui受付終了
				imguiManager->End();

				// 描画開始
				dxCommon->PreDraw();
				// 終了結果画面の描画
			    if (gameScene->IsPlayerAlive() == true) {
				    gameScene->DrawClear();
				} 
				else {
				    gameScene->DrawOver();
				}
				// 軸表示の描画
				axisIndicator->Draw();
				// プリミティブ描画のリセット
				primitiveDrawer->Reset();
				// ImGui描画
				imguiManager->Draw();
				// 描画終了
				dxCommon->PostDraw();

				// クリアの毎フレーム処理
				if (input->TriggerKey(DIK_SPACE)) {
					// 音声停止
				    audio->StopWave(soundHandle);
					SCENE = TITLE;
					break;
				}
			}

		#pragma endregion
		}
	//}
	// 各種解放
	SafeDelete(gameScene);
	// audio解放
	audio->Finalize();
	// ImGui解放
	imguiManager->Finalize();


	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}