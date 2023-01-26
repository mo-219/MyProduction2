#pragma once

#include <string>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>


// 文字描画のアライン
enum class TEXT_ALIGN
{
	UPPER_LEFT = 0, // 左上
	UPPER_MIDDLE,   // 上中央
	UPPER_RIGHT,    // 右上
	MIDDLE_LEFT,    // 左中央
	MIDDLE,         // 中央
	MIDDLE_RIGHT,   // 右中央
	LOWER_LEFT,     // 左下
	LOWER_MIDDLE,   // 下中央
	LOWER_RIGHT,    // 右下
};


//--------------------------------------------------------------
//  
//  スプライトクラス
// 
//--------------------------------------------------------------
class Sprite
{
public:
	Sprite();
	~Sprite() {}

	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	texcoord;
	};

	// バッファ更新
	void Update(
		float dx, float dy,
		float dw, float dh,
		float sx, float sy,
		float sw, float sh,
		float angle,
		float r, float g, float b, float a) const;

	// シェーダーリソースビューの設定
	void SetShaderResourceView(const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv, int texWidth, int texHeight);

	// 頂点バッファの取得
	const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexBuffer() const { return vertexBuffer; }

	// シェーダーリソースビューの取得
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() const { return shaderResourceView; }

	// テクスチャ幅取得
	int GetTextureWidth() const { return textureWidth; }

	// テクスチャ高さ取得
	int GetTextureHeight() const { return textureHeight; }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView;

	int textureWidth = 0;
	int textureHeight = 0;
};



//--------------------------------------------------------------
//  
//  スプライトバッチクラス
// 
//--------------------------------------------------------------

class SpriteBatch
{
private:
    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;
    ID3D11Buffer* buffer = nullptr;
    ID3D11RasterizerState* rasterizerState = nullptr;

    ID3D11ShaderResourceView* shaderResourceView = nullptr;
    D3D11_TEXTURE2D_DESC        tex2dDesc = {};
    ID3D11SamplerState* samplerState = nullptr;
    ID3D11DepthStencilState* depthStencilState = nullptr;

    size_t MAX_INSTANCES;

    //--------------------------------------------------------------
    //  構造体定義
    //--------------------------------------------------------------
    struct instance
    {
        DirectX::XMFLOAT4X4 ndcTransform = {};
        DirectX::XMFLOAT4 texcoordTransform = {};
        DirectX::XMFLOAT4 color = {};
    };
    ID3D11Buffer* instanceBuffer;

public:

    //--------------------------------------------------------------
    //  構造体定義
    //--------------------------------------------------------------
    struct vertex
    {
        DirectX::XMFLOAT3 position = {};
        DirectX::XMFLOAT2 texcoord = {};
    };

    //--------------------------------------------------------------
    //  コンストラクタ
    //--------------------------------------------------------------
    SpriteBatch(ID3D11Device*, const wchar_t*, size_t);

    //--------------------------------------------------------------
    //  デストラクタ
    //--------------------------------------------------------------
    ~SpriteBatch();

    //--------------------------------------------------------------
    //  前処理
    //--------------------------------------------------------------
    void begin(ID3D11DeviceContext*);

    //--------------------------------------------------------------
    //  スプライトバッチ描画
    //--------------------------------------------------------------
    void render(
        const DirectX::XMFLOAT2&, const DirectX::XMFLOAT2&,
        const DirectX::XMFLOAT2&, const DirectX::XMFLOAT2&,
        const DirectX::XMFLOAT2&, float angle,
        const DirectX::XMFLOAT4&
    );

    //--------------------------------------------------------------
    //  テキスト描画
    //--------------------------------------------------------------
    float textout(
        std::string, const DirectX::XMFLOAT2&,
        const DirectX::XMFLOAT2&, const DirectX::XMFLOAT4&,
        TEXT_ALIGN align = (TEXT_ALIGN::UPPER_LEFT), bool world = (false)
    );

    //--------------------------------------------------------------
    //  後処理
    //--------------------------------------------------------------
    void end(ID3D11DeviceContext*);

private:

    D3D11_VIEWPORT viewport;

    UINT instanceCount = 0;
    instance* instances = nullptr;
};