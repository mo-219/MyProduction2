#include "Graphics/Graphics.h"
#include "Graphics/Model.h"

#include "ResourceManager.h"


// コンストラクタ
Model::Model(const char* filename)
{
	// リソース読み込み
	resource = ResourceManager::Instance().LoadModelResource(filename);


	// ノード
	const std::vector<ModelResource::Node>& resNodes = resource->GetNodes();

	nodes.resize(resNodes.size());
	for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
	{
		auto&& src = resNodes.at(nodeIndex);
		auto&& dst = nodes.at(nodeIndex);

		dst.name = src.name.c_str();
		dst.parent = src.parentIndex >= 0 ? &nodes.at(src.parentIndex) : nullptr;
		dst.scale = src.scale;
		dst.rotate = src.rotate;
		dst.translate = src.translate;

		if (dst.parent != nullptr)
		{
			dst.parent->children.emplace_back(&dst);
		}
	}

	// 行列計算
	const DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	UpdateTransform(transform);
}

// 変換行列計算
void Model::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
{
	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

	for (Node& node : nodes)
	{
		node.name;
		// ローカル行列算出
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
		DirectX::XMMATRIX LocalTransform = S * R * T;

		// ワールド行列算出
		DirectX::XMMATRIX ParentTransform;
		if (node.parent != nullptr)
		{
			ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
		}
		else
		{
			ParentTransform = Transform;
		}
		DirectX::XMMATRIX WorldTransform = LocalTransform * ParentTransform;

		// 計算結果を格納
		DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
		DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);

		WorldTransform = LocalTransform * ParentTransform;
	}
}


// アニメーション更新処理
void Model::UpdateAnimation(float elapsedTime)
{
	// 再生中でないなら処理しない
	if (!IsPlayAnimation())		return;

	// ブレンド率の計算
	float blendRate = 1.0f;
	if (animationBlendTime < animationBlendSeconds)
	{
		animationBlendTime += elapsedTime;
		if (animationBlendTime >= animationBlendSeconds)	animationBlendTime = animationBlendSeconds;
		blendRate = animationBlendTime / animationBlendSeconds;

		blendRate *= blendRate;
		// ブレンドレートを二乗
		// 次のノードのほうがより依存する感じ
		// いーじんぐ
	}

	// 指定のアニメーションデータを取得
	const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
	const ModelResource::Animation& animation = animations.at(currentAnimationIndex);

	// アニメーションデータからキーフレームデータリストを取得
	const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
	int keyCount = static_cast<int>(keyframes.size());

	for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
	{
		// 現在の時間がどのキーフレームの間にいるか判定する
		const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1U);

		if (currentAnimationSeconds >= keyframe0.seconds && currentAnimationSeconds < keyframe1.seconds)
		{
			// 再生時間とキーフレームの時間から補完率を算出する
			float rate = (currentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

			int nodeCount = static_cast<int>(nodes.size());

			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
			{
				// 二つのキーフレーム間の補完計算
				const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
				const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

				Node& node = nodes[nodeIndex];

				DirectX::XMVECTOR node0Scale, node0Translate, node0Rotate;
				DirectX::XMVECTOR node1Scale, node1Translate, node1Rotate;
				DirectX::XMVECTOR Scale, Translate, Rotate;


				// ブレンド補完処理
				if (blendRate < 1.0f)
				{
					// 現在の姿勢と次のキーフレームとの姿勢の補完

					// ノード0型変換
					//DirectX::XMVECTOR node0Scale, node0Translate;
					//DirectX::XMVECTOR node0Rotate;
					node0Scale = DirectX::XMLoadFloat3(&node.scale);
					node0Translate = DirectX::XMLoadFloat3(&node.translate);
					node0Rotate = DirectX::XMLoadFloat4(&node.rotate);

					// ノード1型変換
					//DirectX::XMVECTOR node1Scale, node1Translate;
					//DirectX::XMVECTOR node1Rotate;
					node1Scale = DirectX::XMLoadFloat3(&key1.scale);
					node1Translate = DirectX::XMLoadFloat3(&key1.translate);
					node1Rotate = DirectX::XMLoadFloat4(&key1.rotate);



					// 計算した値を保存する変数
					//DirectX::XMVECTOR Scale, Translate;
					//DirectX::XMVECTOR Rotate;
					Scale = DirectX::XMVectorLerp(node0Scale, node1Scale, blendRate);
					Translate = DirectX::XMVectorLerp(node0Translate, node1Translate, blendRate);
					Rotate = DirectX::XMQuaternionSlerp(node0Rotate, node1Rotate, blendRate);

				}
				// 通常の計算
				else
				{
					// 前のキーフレームと次のキーフレームの姿勢を補完
					// ノード0型変換
					//DirectX::XMVECTOR node0Scale, node0Translate;
					//DirectX::XMVECTOR node0Rotate;
					node0Scale = DirectX::XMLoadFloat3(&key0.scale);
					node0Translate = DirectX::XMLoadFloat3(&key0.translate);
					node0Rotate = DirectX::XMLoadFloat4(&key0.rotate);

					// ノード1型変換
					//DirectX::XMVECTOR node1Scale, node1Translate;
					//DirectX::XMVECTOR node1Rotate;
					node1Scale = DirectX::XMLoadFloat3(&key1.scale);
					node1Translate = DirectX::XMLoadFloat3(&key1.translate);
					node1Rotate = DirectX::XMLoadFloat4(&key1.rotate);



					// 計算した値を保存する変数
					//DirectX::XMVECTOR Scale, Translate;
					//DirectX::XMVECTOR Rotate;
					Scale = DirectX::XMVectorLerp(node0Scale, node1Scale, rate);
					Translate = DirectX::XMVectorLerp(node0Translate, node1Translate, rate);
					Rotate = DirectX::XMQuaternionSlerp(node0Rotate, node1Rotate, rate);

				}

				// 値戻す
				DirectX::XMStoreFloat3(&node.scale, Scale);
				DirectX::XMStoreFloat3(&node.translate, Translate);
				DirectX::XMStoreFloat4(&node.rotate, Rotate);


			}
			break;

		}
	}

	// 最終フレーム処理
	if (animationEndFlag)
	{
		animationEndFlag = false;
		currentAnimationIndex = -1;
	}

	// 時間経過
	currentAnimationSeconds += elapsedTime * animationSpeed;

	// 再生時間が週短時間を超えたら
	if (currentAnimationSeconds >= animation.secondsLength)
	{
		// ループするとき
		if (animationLoopFlag)
		{
			// 再生時間を巻き戻す
			currentAnimationSeconds -= animation.secondsLength;
		}

		// ループしないとき 
		else
		{
			currentAnimationSeconds -= animation.secondsLength;
			animationEndFlag = true;
		}
	}

}

//// アニメーション再生
//void Model::PlayAnimation(int index)
//{
//	currentAnimationIndex = index;
//	currentAnimationSeconds = 0.0f;
//}
//void Model::PlayAnimation(int index, bool loop)
//{
//	currentAnimationIndex = index;
//	currentAnimationSeconds = 0.0f;
//	animationLoopFlag = loop;
//	animationEndFlag = false;
//}

void Model::PlayAnimation(int index, bool loop, float speed, float blendSeconds)
{
	currentAnimationIndex = index;
	currentAnimationSeconds = 0.0f;
	animationLoopFlag = loop;
	animationEndFlag = false;

	animationSpeed = speed;
	animationBlendTime = 0.0f;
	animationBlendSeconds = blendSeconds;
}

// アニメーション再生中か
bool Model::IsPlayAnimation() const
{
	if (currentAnimationIndex < 0)									return false;
	if (currentAnimationIndex >= resource->GetAnimations().size())	return false;
	if (animationEndFlag)											return false;

	return true;
}

Model::Node* Model::FindNode(const char* name)
{
	// 全てのノードを総当たりで名前を比較する
	for (Node node : nodes)
	{
		if (strcmp(node.name, name) == 0)	// 文字列(const char*)の比較は strcmp()を使う

		{
			return &node;
		}
	}

	return nullptr;			// 見つからなかったとき
}
