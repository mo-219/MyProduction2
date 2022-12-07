#include "Graphics/Graphics.h"
#include "Graphics/Model.h"

#include "ResourceManager.h"


// �R���X�g���N�^
Model::Model(const char* filename)
{
	// ���\�[�X�ǂݍ���
	resource = ResourceManager::Instance().LoadModelResource(filename);


	// �m�[�h
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

	// �s��v�Z
	const DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	UpdateTransform(transform);
}

// �ϊ��s��v�Z
void Model::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
{
	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

	for (Node& node : nodes)
	{
		node.name;
		// ���[�J���s��Z�o
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
		DirectX::XMMATRIX LocalTransform = S * R * T;

		// ���[���h�s��Z�o
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

		// �v�Z���ʂ��i�[
		DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
		DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);

		WorldTransform = LocalTransform * ParentTransform;
	}
}


// �A�j���[�V�����X�V����
void Model::UpdateAnimation(float elapsedTime)
{
	// �Đ����łȂ��Ȃ珈�����Ȃ�
	if (!IsPlayAnimation())		return;

	// �u�����h���̌v�Z
	float blendRate = 1.0f;
	if (animationBlendTime < animationBlendSeconds)
	{
		animationBlendTime += elapsedTime;
		if (animationBlendTime >= animationBlendSeconds)	animationBlendTime = animationBlendSeconds;
		blendRate = animationBlendTime / animationBlendSeconds;

		blendRate *= blendRate;
		// �u�����h���[�g����
		// ���̃m�[�h�̂ق������ˑ����銴��
		// ���[����
	}

	// �w��̃A�j���[�V�����f�[�^���擾
	const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
	const ModelResource::Animation& animation = animations.at(currentAnimationIndex);

	// �A�j���[�V�����f�[�^����L�[�t���[���f�[�^���X�g���擾
	const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
	int keyCount = static_cast<int>(keyframes.size());

	for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
	{
		// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
		const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1U);

		if (currentAnimationSeconds >= keyframe0.seconds && currentAnimationSeconds < keyframe1.seconds)
		{
			// �Đ����ԂƃL�[�t���[���̎��Ԃ���⊮�����Z�o����
			float rate = (currentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

			int nodeCount = static_cast<int>(nodes.size());

			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
			{
				// ��̃L�[�t���[���Ԃ̕⊮�v�Z
				const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
				const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

				Node& node = nodes[nodeIndex];

				DirectX::XMVECTOR node0Scale, node0Translate, node0Rotate;
				DirectX::XMVECTOR node1Scale, node1Translate, node1Rotate;
				DirectX::XMVECTOR Scale, Translate, Rotate;


				// �u�����h�⊮����
				if (blendRate < 1.0f)
				{
					// ���݂̎p���Ǝ��̃L�[�t���[���Ƃ̎p���̕⊮

					// �m�[�h0�^�ϊ�
					//DirectX::XMVECTOR node0Scale, node0Translate;
					//DirectX::XMVECTOR node0Rotate;
					node0Scale = DirectX::XMLoadFloat3(&node.scale);
					node0Translate = DirectX::XMLoadFloat3(&node.translate);
					node0Rotate = DirectX::XMLoadFloat4(&node.rotate);

					// �m�[�h1�^�ϊ�
					//DirectX::XMVECTOR node1Scale, node1Translate;
					//DirectX::XMVECTOR node1Rotate;
					node1Scale = DirectX::XMLoadFloat3(&key1.scale);
					node1Translate = DirectX::XMLoadFloat3(&key1.translate);
					node1Rotate = DirectX::XMLoadFloat4(&key1.rotate);



					// �v�Z�����l��ۑ�����ϐ�
					//DirectX::XMVECTOR Scale, Translate;
					//DirectX::XMVECTOR Rotate;
					Scale = DirectX::XMVectorLerp(node0Scale, node1Scale, blendRate);
					Translate = DirectX::XMVectorLerp(node0Translate, node1Translate, blendRate);
					Rotate = DirectX::XMQuaternionSlerp(node0Rotate, node1Rotate, blendRate);

				}
				// �ʏ�̌v�Z
				else
				{
					// �O�̃L�[�t���[���Ǝ��̃L�[�t���[���̎p����⊮
					// �m�[�h0�^�ϊ�
					//DirectX::XMVECTOR node0Scale, node0Translate;
					//DirectX::XMVECTOR node0Rotate;
					node0Scale = DirectX::XMLoadFloat3(&key0.scale);
					node0Translate = DirectX::XMLoadFloat3(&key0.translate);
					node0Rotate = DirectX::XMLoadFloat4(&key0.rotate);

					// �m�[�h1�^�ϊ�
					//DirectX::XMVECTOR node1Scale, node1Translate;
					//DirectX::XMVECTOR node1Rotate;
					node1Scale = DirectX::XMLoadFloat3(&key1.scale);
					node1Translate = DirectX::XMLoadFloat3(&key1.translate);
					node1Rotate = DirectX::XMLoadFloat4(&key1.rotate);



					// �v�Z�����l��ۑ�����ϐ�
					//DirectX::XMVECTOR Scale, Translate;
					//DirectX::XMVECTOR Rotate;
					Scale = DirectX::XMVectorLerp(node0Scale, node1Scale, rate);
					Translate = DirectX::XMVectorLerp(node0Translate, node1Translate, rate);
					Rotate = DirectX::XMQuaternionSlerp(node0Rotate, node1Rotate, rate);

				}

				// �l�߂�
				DirectX::XMStoreFloat3(&node.scale, Scale);
				DirectX::XMStoreFloat3(&node.translate, Translate);
				DirectX::XMStoreFloat4(&node.rotate, Rotate);


			}
			break;

		}
	}

	// �ŏI�t���[������
	if (animationEndFlag)
	{
		animationEndFlag = false;
		currentAnimationIndex = -1;
	}

	// ���Ԍo��
	currentAnimationSeconds += elapsedTime * animationSpeed;

	// �Đ����Ԃ��T�Z���Ԃ𒴂�����
	if (currentAnimationSeconds >= animation.secondsLength)
	{
		// ���[�v����Ƃ�
		if (animationLoopFlag)
		{
			// �Đ����Ԃ������߂�
			currentAnimationSeconds -= animation.secondsLength;
		}

		// ���[�v���Ȃ��Ƃ� 
		else
		{
			currentAnimationSeconds -= animation.secondsLength;
			animationEndFlag = true;
		}
	}

}

//// �A�j���[�V�����Đ�
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

// �A�j���[�V�����Đ�����
bool Model::IsPlayAnimation() const
{
	if (currentAnimationIndex < 0)									return false;
	if (currentAnimationIndex >= resource->GetAnimations().size())	return false;
	if (animationEndFlag)											return false;

	return true;
}

Model::Node* Model::FindNode(const char* name)
{
	// �S�Ẵm�[�h�𑍓�����Ŗ��O���r����
	for (Node node : nodes)
	{
		if (strcmp(node.name, name) == 0)	// ������(const char*)�̔�r�� strcmp()���g��

		{
			return &node;
		}
	}

	return nullptr;			// ������Ȃ������Ƃ�
}
