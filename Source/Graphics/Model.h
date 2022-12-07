#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "Graphics/ModelResource.h"

// モデル
class Model
{
public:
	Model(const char* filename);
	~Model() {}

	struct Node
	{
		const char*			name;
		Node*				parent;
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;
		DirectX::XMFLOAT4X4	localTransform;
		DirectX::XMFLOAT4X4	worldTransform;

		std::vector<Node*>	children;
	};

	// 行列計算
	void UpdateTransform(const DirectX::XMFLOAT4X4& transform);

	// ノードリスト取得
	const std::vector<Node>& GetNodes() const { return nodes; }
	std::vector<Node>& GetNodes() { return nodes; }

	// リソース取得
	const ModelResource* GetResource() const { return resource.get(); }

//--------------------------------------------------------------
//
//			アニメーション
//
//--------------------------------------------------------------
	void UpdateAnimation(float elapsedTime);							// 更新処理
	void PlayAnimation(int index, bool loop, float speed = 1.0f, float blendSeconds = 0.2f);	// 再生
	bool IsPlayAnimation() const;										// 再生中かどうか	
	float GetCurrentAnimetionSeconds() const
	{
		return currentAnimationSeconds;
	}								// 現在の再生時間取得

// ノード検索
	Node* FindNode(const char* name);


private:
	std::shared_ptr<ModelResource>	resource;
	std::vector<Node>				nodes;


	int		currentAnimationIndex = -1;
	float	currentAnimationSeconds = 0.0f;
	float	animationSpeed = 1.0f;

	bool	animationLoopFlag = false;
	bool	animationEndFlag = false;

	float	animationBlendTime = 0.0f;
	float	animationBlendSeconds = 0.0f;
};
