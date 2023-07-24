#pragma once
#include <iostream>
#include <list>
#include "Collider.h"

class CollisionManager {
public:
	/// <summary>
	/// コライダーを受け取る
	/// </summary>
	/// <param name="collider">コライダー</param>
	void GetCollider(Collider* collider) { collider_.push_back(collider); }
	
	/// <summary>
	/// コライダーをクリアする
	/// </summary>
	void ClearCollider() { collider_.clear(); }

	/// <summary>
	/// AとBの距離を取得
	/// </summary>
	/// <param name="A">一つ目の要素</param>
	/// <param name="B">二つ目の要素</param>
	/// <returns>二点間の距離</returns>
	float GetDistance(const Vector3& A, const Vector3& B) {
		float distance = (B.x - A.x) * (B.x - A.x) +
		                 (B.y - A.y) * (B.y - A.y) +
		                 (B.z - A.z) * (B.z - A.z);
		return distance;
	}

	/// <summary>
	/// コライダー2つの衝突判定と応答
	/// </summary>
	/// <param name="colliderA">コライダーA</param>
	/// <param name="colliderB">コライダーB</param>
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
		// 衝突フィルタリング
		if (colliderA->GetCollisionAttribute() != colliderB->GetCollisionMask() ||
		    colliderB->GetCollisionAttribute() != colliderA->GetCollisionMask()) {
			return;
		}

		float distance = GetDistance(colliderA->GetWorldPosition(), colliderB->GetWorldPosition());
		// 球と球の交差判定
		if (distance <= (colliderA->GetRadius() + colliderB->GetRadius()) *
		                    (colliderA->GetRadius() + colliderB->GetRadius())) {
			// コライダーAの衝突時コールバックを呼び出す
			colliderA->OnCollision();
			// コライダーBの衝突時コールバックを呼び出す
			colliderB->OnCollision();
		}
	}

	/// <summary>
	/// 全体の当たり判定
	/// </summary>
	void CheckAllCollisions(){
		// リスト内のペアを総当たり
		std::list<Collider*>::iterator itrA = collider_.begin();
		for (; itrA != collider_.end(); ++itrA) {
			// イテレータAからコライダーAを取得する
			Collider* A = *itrA;
			// イテレータBはイテレータAの次の要素から回す（重複判定を回避）
			std::list<Collider*>::iterator itrB = itrA;
			itrB++;
			for (; itrB != collider_.end(); ++itrB) {
				// イテレータBからコライダーBを取得する
				Collider* B = *itrB;
				// コライダーAとコライダーBの当たり判定
				CheckCollisionPair(A, B);
			}
		}
	}

private:
	// コライダーリスト
	std::list<Collider*> collider_;
};
