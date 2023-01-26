#pragma once

#include <string>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>


// �����`��̃A���C��
enum class TEXT_ALIGN
{
	UPPER_LEFT = 0, // ����
	UPPER_MIDDLE,   // �㒆��
	UPPER_RIGHT,    // �E��
	MIDDLE_LEFT,    // ������
	MIDDLE,         // ����
	MIDDLE_RIGHT,   // �E����
	LOWER_LEFT,     // ����
	LOWER_MIDDLE,   // ������
	LOWER_RIGHT,    // �E��
};


//--------------------------------------------------------------
//  
//  �X�v���C�g�N���X
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

	// �o�b�t�@�X�V
	void Update(
		float dx, float dy,
		float dw, float dh,
		float sx, float sy,
		float sw, float sh,
		float angle,
		float r, float g, float b, float a) const;

	// �V�F�[�_�[���\�[�X�r���[�̐ݒ�
	void SetShaderResourceView(const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv, int texWidth, int texHeight);

	// ���_�o�b�t�@�̎擾
	const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexBuffer() const { return vertexBuffer; }

	// �V�F�[�_�[���\�[�X�r���[�̎擾
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() const { return shaderResourceView; }

	// �e�N�X�`�����擾
	int GetTextureWidth() const { return textureWidth; }

	// �e�N�X�`�������擾
	int GetTextureHeight() const { return textureHeight; }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView;

	int textureWidth = 0;
	int textureHeight = 0;
};



//--------------------------------------------------------------
//  
//  �X�v���C�g�o�b�`�N���X
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
    //  �\���̒�`
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
    //  �\���̒�`
    //--------------------------------------------------------------
    struct vertex
    {
        DirectX::XMFLOAT3 position = {};
        DirectX::XMFLOAT2 texcoord = {};
    };

    //--------------------------------------------------------------
    //  �R���X�g���N�^
    //--------------------------------------------------------------
    SpriteBatch(ID3D11Device*, const wchar_t*, size_t);

    //--------------------------------------------------------------
    //  �f�X�g���N�^
    //--------------------------------------------------------------
    ~SpriteBatch();

    //--------------------------------------------------------------
    //  �O����
    //--------------------------------------------------------------
    void begin(ID3D11DeviceContext*);

    //--------------------------------------------------------------
    //  �X�v���C�g�o�b�`�`��
    //--------------------------------------------------------------
    void render(
        const DirectX::XMFLOAT2&, const DirectX::XMFLOAT2&,
        const DirectX::XMFLOAT2&, const DirectX::XMFLOAT2&,
        const DirectX::XMFLOAT2&, float angle,
        const DirectX::XMFLOAT4&
    );

    //--------------------------------------------------------------
    //  �e�L�X�g�`��
    //--------------------------------------------------------------
    float textout(
        std::string, const DirectX::XMFLOAT2&,
        const DirectX::XMFLOAT2&, const DirectX::XMFLOAT4&,
        TEXT_ALIGN align = (TEXT_ALIGN::UPPER_LEFT), bool world = (false)
    );

    //--------------------------------------------------------------
    //  �㏈��
    //--------------------------------------------------------------
    void end(ID3D11DeviceContext*);

private:

    D3D11_VIEWPORT viewport;

    UINT instanceCount = 0;
    instance* instances = nullptr;
};