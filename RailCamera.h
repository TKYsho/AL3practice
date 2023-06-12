#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"

/// <summary>
/// レールカメラ
/// </summary>
class RailCamera {

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Vector3& position, const Vector3& radian);

	/// <summary>
	/// 更新
	/// <summary>
	void Update();

	/// <summary>
	/// ビュープロジェクションを取得
	/// </summary>
	/// <returns>ビュープロジェクション</returns>
	const ViewProjection& GetViewProjection() { return viewProjection_; }

	/// <summary>
	/// ワールドトランスフォームを取得
	/// </summary>
	/// <returns>ワールドトランスフォーム</returns>
	const WorldTransform& GetWorldTransform() { return worldTransform_; }


private:
	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	// ビュープロジェクション
	ViewProjection viewProjection_;

};

