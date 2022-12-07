#pragma once

#include "Graphics/Graphics.h"
#include "Graphics/Shader.h"
#include "LoadObj.h"

#include<DirectXMATH.h>
#include<string>
#include"Graphics/Model.h"

#include "Graphics/Shader.h"
#include "Param.h"

class GameObject
{
public:
	GameObject() {};
	~GameObject();


	void CreateModel(const char* filename);

	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext* context, Shader* shader);
	void Render(const RenderContext& rc, ModelShader* shader);

	//行列更新処理
	void UpdateTransform();

	void UpdateOnlyTransform(float elapsedTime);



	RenderContext SetRenderContext(const RenderContext& rc);


	void Initialize();

	virtual void DrawDebugPrimitive() {};

	virtual void DrawDebugImGui() ;

	void SetCode(std::string str) { code = str; }
	void SetScale(DirectX::XMFLOAT3 s) { scale = s; }
	void SetAngle(DirectX::XMFLOAT3 r) { angle = r; }
	void SetPosition(DirectX::XMFLOAT3 p) { param.position = p; }
	void SetBehavior(CollisionBehavior* be) { behavior = be; }
	void SetRayCast(RayCastBehavior* ray) { rayCast = ray; }

	void SetParam(Param para) { param = para; }
	void SetRadius(float ra) { param.radius = ra; }


	std::string GetCode() { return code; }
	DirectX::XMFLOAT3 GetScale() { return scale; }
	DirectX::XMFLOAT3 GetAngle() { return angle; }
	DirectX::XMFLOAT3 GetPosition() { return param.position; }
	float GetRadius() { return param.radius; }
	float GetWidth() { return param.width; }
	float GetHeight() { return param.height; }
	float GetDepth() { return param.depth; }

	CollisionBehavior* GetBehavior() { return behavior; }
	RayCastBehavior* GetRayCast() { return rayCast; }
	Param GetParam() { return param; }
	Model* GetModel() { return model; }

public:
	// 一旦パブリックにしてる
	Model* model = nullptr;

	std::string code = {};
	DirectX::XMFLOAT3 scale = {};
	DirectX::XMFLOAT3 angle = {};
	//DirectX::XMFLOAT3 position = {};

	DirectX::XMFLOAT4X4 transform = {
	1,0,0,0,    // X軸ベクトル(右方向ベクトルのx,y,z(右はどっちかがわかる))
	0,1,0,0,    // Y軸ベクトル(上方向のベクトル)
	0,0,1,0,    // Z軸ベクトル(前方向のベクトル)
	0,0,0,1     // 位置
	};

	// あたり判定
	CollisionBehavior* behavior;
	RayCastBehavior* rayCast;

	Param param = { DirectX::XMFLOAT3(0,0,0), 0.5f, 0.0f,2.0f,0.0f };

public:
	// デバッグ用
	bool CollisionFlag = false;

};
