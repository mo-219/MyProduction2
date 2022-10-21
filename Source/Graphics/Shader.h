#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Graphics/RenderContext.h"
#include "Graphics/Model.h"
#include "Graphics/Sprite.h"

HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);
HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);




class Shader
{
public:
	Shader() {}
	virtual ~Shader() {}

	// •`‰æŠJn
	virtual void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) = 0;

	// •`‰æ
	virtual void Draw(ID3D11DeviceContext* dc, const Model* model) = 0;

	// •`‰æI—¹
	virtual void End(ID3D11DeviceContext* context) = 0;
};



class ModelShader
{
public:
	ModelShader() {}
	virtual ~ModelShader() {}

	// •`‰æŠJn
	virtual void Begin(const RenderContext& rc) = 0;

	// •`‰æ
	virtual void Draw(const RenderContext& rc, const Model* model) = 0;

	// •`‰æI—¹
	virtual void End(const RenderContext& rc) = 0;

	virtual 	void DebugGUI() {};
};

class SpriteShader
{
public:
	SpriteShader() {}
	virtual ~SpriteShader() {}

	// •`‰æŠJn
	virtual void Begin(const RenderContext& rc) = 0;

	// •`‰æ
	virtual void Draw(const RenderContext& rc, const Sprite* sprite) = 0;

	// •`‰æI—¹
	virtual void End(const RenderContext& rc) = 0;
};

