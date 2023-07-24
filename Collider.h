#pragma once
#include "MathUtility.h"

/// <summary>
/// 衝突判定オブジェクト
/// </summary>
class Collider {
public:
	// 半径を取得
	float GetRadius() { return radius_; }
	// 半径を設定
	void SetRadius(float radius) { radius_ = radius; }
	// 衝突時に呼ばれる関数
	virtual void OnCollision(){};
	// ワールド座標を取得
	virtual Vector3 GetWorldPosition(){
		return {
		    worldTransform_.matWorld_.m[3][0], 
			worldTransform_.matWorld_.m[3][1],
		    worldTransform_.matWorld_.m[3][2]
		};
	}

private:
	// 衝突半径
	float radius_ = 0;
	WorldTransform worldTransform_;
};