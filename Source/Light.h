#pragma once
#include <DirectXMath.h>

// 光源タイプ
enum class LightType
{
	Directional,	// 平行光源
};

// 光源クラス
class Light
{
public:
	Light(LightType lightType = LightType::Directional);

	// デバッグ情報の表示
	void DrawDebugGUI();

	// デバッグ図形の表示
	void DrawDebugPrimitive();

	// ライトの向き
	DirectX::XMFLOAT3 GetDirection() { return direction; }
	void SetDirection(DirectX::XMFLOAT3 direction) { this->direction = direction; }

	// 色
	DirectX::XMFLOAT4 GetColor() { return color; }
	void SetColor(DirectX::XMFLOAT4 color) { this->color = color; }

private:
	LightType			lightType = LightType::Directional;		// ライトタイプ
	DirectX::XMFLOAT3	direction = DirectX::XMFLOAT3(0, -1, -1);	// ライトの向き
	DirectX::XMFLOAT4	color = DirectX::XMFLOAT4(1, 1, 1, 1);	// ライトの色
};
